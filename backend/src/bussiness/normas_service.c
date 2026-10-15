#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>
#include <math.h>
#include "../system/hash.h"
#include "../system/routes.h"
#include "../system/db.h"
#include "../system/commonlib.h"
#include "../system/ArrayList.h"
#include "../system/open_docs.h"
#include "normas_service.h"

static ArrayList* pListNormasLocal = NULL;

// ===================================================================================================================================== //
// Codificador y Decodificador Base64 (Universal)
// ===================================================================================================================================== //
static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    *output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = (char *)malloc(*output_length + 1);
    if (!encoded_data) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        encoded_data[j++] = b64_table[(triple >> 18) & 0x3F];
        encoded_data[j++] = b64_table[(triple >> 12) & 0x3F];
        encoded_data[j++] = (i > input_length + 1) ? '=' : b64_table[(triple >> 6) & 0x3F];
        encoded_data[j++] = (i > input_length) ? '=' : b64_table[triple & 0x3F];
    }
    encoded_data[*output_length] = '\0';
    return encoded_data;
}

static unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length) {
    if (input_length % 4 != 0) return NULL;

    *output_length = (input_length / 4) * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = (unsigned char *)malloc(*output_length);
    if (!decoded_data) return NULL;

    static char decoding_table[256];
    static int table_built = 0;
    if (!table_built) {
        memset(decoding_table, 0x80, 256);
        for (int i = 0; i < 64; i++) decoding_table[(unsigned char)b64_table[i]] = (char)i;
        decoding_table['='] = 0;
        table_built = 1;
    }

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t a = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)data[i++]];
        uint32_t b = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)data[i++]];
        uint32_t c = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)data[i++]];
        uint32_t d = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned char)data[i++]];

        uint32_t triple = (a << 18) + (b << 12) + (c << 6) + d;

        if (j < *output_length) decoded_data[j++] = (triple >> 16) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = triple & 0xFF;
    }
    return decoded_data;
}

// ===================================================================================================================================== //
// Manejo Físico de Archivos Subidos
// ===================================================================================================================================== //
static void get_target_upload_disk_path(const char *filename, char *out_disk_path, size_t max_len) {
    char exe_path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", exe_path, PATH_MAX);

    if (count != -1) {
        exe_path[count] = '\0';
        char *exe_dir = dirname(exe_path);

        char temp_dir[PATH_MAX];
        snprintf(temp_dir, sizeof(temp_dir), "%s/../../uploads", exe_dir);

        char resolved_dir[PATH_MAX];
        if (realpath(temp_dir, resolved_dir) != NULL) {
            snprintf(out_disk_path, max_len, "%s/%s", resolved_dir, filename);
        } else {
            snprintf(out_disk_path, max_len, "%s/%s", temp_dir, filename);
        }
    } else {
        snprintf(out_disk_path, max_len, "../../uploads/%s", filename);
    }
}

static int save_upload_file(const char *target_path, const char *b64_content) {
    if (!target_path || strlen(target_path) == 0 || !b64_content || strlen(b64_content) == 0) {
        return 0;
    }

    const char *payload = strstr(b64_content, "base64,");
    if (payload) {
        payload += 7;
    } else {
        payload = b64_content;
    }

    size_t out_len = 0;
    unsigned char *decoded = base64_decode(payload, strlen(payload), &out_len);
    if (!decoded) return 0;

    FILE *f = fopen(target_path, "wb");
    if (!f) {
        free(decoded);
        printf("🚨 [UPLOADS] No se pudo abrir para escritura: %s\n", target_path);
        return 0;
    }

    fwrite(decoded, 1, out_len, f);
    fclose(f);
    free(decoded);

    printf("📄 [UPLOADS] Archivo guardado físicamente en: %s (%zu bytes)\n", target_path, out_len);
    return 1;
}

// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES
// ===================================================================================================================================== //
static int sp_insertar_normas(const char* nombre_norma, const char* n_norma, const char* tipo_norma, const char* f_norma, const char* f_pub, const char* anio_pub, const char* jurisdiccion, const char* organismo, const char* unidad_fisica, const char* observaciones, const char* file_name, const char* file_path) {

    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[12];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_normas(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) {
        if (stmt) mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_param, 0, sizeof(bind_param));

    bind_param[0].buffer_type = MYSQL_TYPE_STRING;
    bind_param[0].buffer = (char *)nombre_norma;
    bind_param[0].buffer_length = strlen(nombre_norma);

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)n_norma;
    bind_param[1].buffer_length = strlen(n_norma);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)tipo_norma;
    bind_param[2].buffer_length = strlen(tipo_norma);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)f_norma;
    bind_param[3].buffer_length = strlen(f_norma);

    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = (char *)f_pub;
    bind_param[4].buffer_length = strlen(f_pub);

    bind_param[5].buffer_type = MYSQL_TYPE_STRING;
    bind_param[5].buffer = (char *)anio_pub;
    bind_param[5].buffer_length = strlen(anio_pub);

    bind_param[6].buffer_type = MYSQL_TYPE_STRING;
    bind_param[6].buffer = (char *)jurisdiccion;
    bind_param[6].buffer_length = strlen(jurisdiccion);

    bind_param[7].buffer_type = MYSQL_TYPE_STRING;
    bind_param[7].buffer = (char *)organismo;
    bind_param[7].buffer_length = strlen(organismo);

    bind_param[8].buffer_type = MYSQL_TYPE_STRING;
    bind_param[8].buffer = (char *)unidad_fisica;
    bind_param[8].buffer_length = strlen(unidad_fisica);

    bind_param[9].buffer_type = MYSQL_TYPE_STRING;
    bind_param[9].buffer = (char *)observaciones;
    bind_param[9].buffer_length = strlen(observaciones);

    bind_param[10].buffer_type = MYSQL_TYPE_STRING;
    bind_param[10].buffer = (char *)file_name;
    bind_param[10].buffer_length = strlen(file_name);

    bind_param[11].buffer_type = MYSQL_TYPE_STRING;
    bind_param[11].buffer = (char *)file_path;
    bind_param[11].buffer_length = strlen(file_path);

    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_result, 0, sizeof(bind_result));
    bind_result[0].buffer_type = MYSQL_TYPE_LONG;
    bind_result[0].buffer = &nuevo_id;

    if (mysql_stmt_bind_result(stmt, bind_result)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    mysql_stmt_fetch(stmt);
    mysql_stmt_free_result(stmt);
    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);
    return nuevo_id;
}

static int sp_editar_normas(int id, const char* nombre_norma, const char* n_norma, const char* tipo_norma, const char* f_norma, const char* f_pub, const char* anio_pub, const char* jurisdiccion, const char* organismo, const char* unidad_fisica, const char* observaciones, const char* file_name, const char* file_path) {

    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[13];
    const char *query = "CALL sp_editar_normas(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) {
        if (stmt) mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_param, 0, sizeof(bind_param));

    bind_param[0].buffer_type = MYSQL_TYPE_LONG;
    bind_param[0].buffer = (void *)&id;
    bind_param[0].is_unsigned = 0;

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)nombre_norma;
    bind_param[1].buffer_length = strlen(nombre_norma);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)n_norma;
    bind_param[2].buffer_length = strlen(n_norma);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)tipo_norma;
    bind_param[3].buffer_length = strlen(tipo_norma);

    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = (char *)f_norma;
    bind_param[4].buffer_length = strlen(f_norma);

    bind_param[5].buffer_type = MYSQL_TYPE_STRING;
    bind_param[5].buffer = (char *)f_pub;
    bind_param[5].buffer_length = strlen(f_pub);

    bind_param[6].buffer_type = MYSQL_TYPE_STRING;
    bind_param[6].buffer = (char *)anio_pub;
    bind_param[6].buffer_length = strlen(anio_pub);

    bind_param[7].buffer_type = MYSQL_TYPE_STRING;
    bind_param[7].buffer = (char *)jurisdiccion;
    bind_param[7].buffer_length = strlen(jurisdiccion);

    bind_param[8].buffer_type = MYSQL_TYPE_STRING;
    bind_param[8].buffer = (char *)organismo;
    bind_param[8].buffer_length = strlen(organismo);

    bind_param[9].buffer_type = MYSQL_TYPE_STRING;
    bind_param[9].buffer = (char *)unidad_fisica;
    bind_param[9].buffer_length = strlen(unidad_fisica);

    bind_param[10].buffer_type = MYSQL_TYPE_STRING;
    bind_param[10].buffer = (char *)observaciones;
    bind_param[10].buffer_length = strlen(observaciones);

    bind_param[11].buffer_type = MYSQL_TYPE_STRING;
    bind_param[11].buffer = (char *)file_name;
    bind_param[11].buffer_length = strlen(file_name);

    bind_param[12].buffer_type = MYSQL_TYPE_STRING;
    bind_param[12].buffer = (char *)file_path;
    bind_param[12].buffer_length = strlen(file_path);


    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    mysql_close(conn);
    return 1;
}

// ===================================================================================================================================== //
// CONSTRUCTOR Y CACHE
// ===================================================================================================================================== //
Normas* newNormas(){
    Normas* nNorma = (Normas*)malloc(sizeof(Normas));
    if(nNorma != NULL){
        memset(nNorma, 0, sizeof(Normas));
    }
    return nNorma;
}

void normas_init_cache(ArrayList* alistNormas) {
    if(alistNormas != NULL) {
        pListNormasLocal = alistNormas;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ NORMAS ].\n");
    } else {
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ NORMAS ] con NULL.\n");
    }
}

void normas_load_storage(ArrayList* alistNormas) {
    if(alistNormas == NULL) return;

    DBResult *res = db_query("SELECT n.*, o.descripcion FROM normas n LEFT JOIN organismos o ON n.organismo = o.cod_org");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Normas* nNorma = newNormas();
        if (nNorma != NULL) {
            nNorma->id = row[0] ? atoi(row[0]) : 0;
            strncpy(nNorma->nombre_norma, row[1] ? row[1] : "", sizeof(nNorma->nombre_norma) -1);
            strncpy(nNorma->n_norma, row[2] ? row[2] : "", sizeof(nNorma->n_norma) -1);
            strncpy(nNorma->tipo_norma, row[3] ? row[3] : "", sizeof(nNorma->tipo_norma) -1);
            strncpy(nNorma->f_norma, row[4] ? row[4] : "", sizeof(nNorma->f_norma) -1);
            strncpy(nNorma->f_pub, row[5] ? row[5] : "", sizeof(nNorma->f_pub) -1);
            strncpy(nNorma->anio_pub, row[6] ? row[6] : "", sizeof(nNorma->anio_pub) -1);
            strncpy(nNorma->jurisdiccion, row[7] ? row[7] : "", sizeof(nNorma->jurisdiccion) -1);
            strncpy(nNorma->organismo, row[8] ? row[8] : "", sizeof(nNorma->organismo) -1);
            strncpy(nNorma->unidad_fisica, row[9] ? row[9] : "", sizeof(nNorma->unidad_fisica) -1);
            strncpy(nNorma->observaciones, row[10] ? row[10] : "", sizeof(nNorma->observaciones) -1);
            strncpy(nNorma->file_name, row[11] ? row[11] : "", sizeof(nNorma->file_name) -1);
            strncpy(nNorma->file_path, row[12] ? row[12] : "", sizeof(nNorma->file_path) -1);
            strncpy(nNorma->descripcion_organismo, row[13] ? row[13] : "", sizeof(nNorma->descripcion_organismo) -1);

            alistNormas->add(alistNormas, nNorma);
        }
    }
    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d NORMAS cargadas. Espacio reservado: %d slots.\n", alistNormas->len(alistNormas), alistNormas->reservedSize);
}

// ===================================================================================================================================== //
// FUNCIONES AUXILIARES
// ===================================================================================================================================== //
static void get_norma_value(const char *body, const char *key, char *out, size_t out_size) {
    char *pos = strstr(body, key);
    if (!pos) {
        out[0] = '\0';
        return;
    }
    pos += strlen(key);
    if (*pos == '=') pos++;
    const char *end = strchr(pos, '&');
    size_t len = end ? (size_t)(end - pos) : strlen(pos);
    if (len >= out_size) len = out_size - 1;
    strncpy(out, pos, len);
    out[len] = '\0';
}

static char *get_dynamic_value(const char *body, const char *key) {
    char *pos = strstr(body, key);
    if (!pos) return NULL;
    pos += strlen(key);
    if (*pos == '=') pos++;
    const char *end = strchr(pos, '&');
    size_t len = end ? (size_t)(end - pos) : strlen(pos);
    char *out = (char *)malloc(len + 1);
    if (out) {
        strncpy(out, pos, len);
        out[len] = '\0';
    }
    return out;
}

static void json_escape_string(const char *src, char *dest, size_t max_len) {
    size_t j = 0;
    for (size_t i = 0; src[i] != '\0' && j < max_len - 2; i++) {
        switch (src[i]) {
            case '\"': dest[j++] = '\\'; dest[j++] = '\"'; break;
            case '\\': dest[j++] = '\\'; dest[j++] = '\\'; break;
            case '\b': dest[j++] = '\\'; dest[j++] = 'b'; break;
            case '\f': dest[j++] = '\\'; dest[j++] = 'f'; break;
            case '\n': dest[j++] = '\\'; dest[j++] = 'n'; break;
            case '\r': dest[j++] = '\\'; dest[j++] = 'r'; break;
            case '\t': dest[j++] = '\\'; dest[j++] = 't'; break;
            default:
                if ((unsigned char)src[i] < '\x20') {
                    // Ignorar caracteres de control inválidos
                } else {
                    dest[j++] = src[i];
                }
                break;
        }
    }
    dest[j] = '\0';
}

// ===================================================================================================================================== //
// FUNCION PARA REGISTRAR NUEVA NORMA
// ===================================================================================================================================== //
int normas_service_register(const char *body, char *error_msg, int error_size) {
    // 🔹 CORRECCIÓN CRÍTICA: Se inicializan todos los arrays a ceros {0} para evitar arrastrar basura
    char nombre_norma[501] = {0}; char d_nombre_norma[501] = {0};
    char n_norma[101] = {0};      char d_n_norma[101] = {0};
    char tipo_norma[201] = {0};   char d_tipo_norma[201] = {0};
    char f_norma[201] = {0};      char d_f_norma[201] = {0};
    char f_pub[11] = {0};         char d_f_pub[11] = {0};
    char anio_pub[5] = {0};       char d_anio_pub[5] = {0};
    char jurisdiccion[101] = {0}; char d_jurisdiccion[101] = {0};
    char organismo[101] = {0};    char d_organismo[101] = {0};
    char unidad_fisica[101] = {0};char d_unidad_fisica[101] = {0};
    char observaciones[3000] = {0};char d_observaciones[3000] = {0};
    char file_name[101] = {0};    char d_file_name[101] = {0};
    char d_file_path[PATH_MAX] = {0};

    get_norma_value(body,"nombre_norma", nombre_norma, sizeof(nombre_norma)); url_decode(d_nombre_norma, nombre_norma);
    get_norma_value(body, "n_norma", n_norma, sizeof(n_norma));             url_decode(d_n_norma, n_norma);
    get_norma_value(body, "tipo_norma", tipo_norma, sizeof(tipo_norma));    url_decode(d_tipo_norma, tipo_norma);
    get_norma_value(body, "f_norma", f_norma, sizeof(f_norma));             url_decode(d_f_norma, f_norma);
    get_norma_value(body, "f_pub", f_pub, sizeof(f_pub));                   url_decode(d_f_pub, f_pub);
    get_norma_value(body, "anio_pub", anio_pub, sizeof(anio_pub));          url_decode(d_anio_pub, anio_pub);
    get_norma_value(body, "jurisdiccion", jurisdiccion, sizeof(jurisdiccion));url_decode(d_jurisdiccion, jurisdiccion);
    get_norma_value(body, "organismo", organismo, sizeof(organismo));       url_decode(d_organismo, organismo);
    get_norma_value(body, "unidad_fisica", unidad_fisica, sizeof(unidad_fisica));url_decode(d_unidad_fisica, unidad_fisica);
    get_norma_value(body, "observaciones", observaciones, sizeof(observaciones));url_decode(d_observaciones, observaciones);
    get_norma_value(body, "file_name", file_name, sizeof(file_name));       url_decode(d_file_name, file_name);

    get_target_upload_disk_path(d_file_name, d_file_path, sizeof(d_file_path));

    if (strlen(d_nombre_norma) == 0 || strlen(d_n_norma) == 0 || strlen(d_tipo_norma) == 0 || strlen(d_f_norma) == 0 || strlen(d_f_pub) == 0 || strlen(d_anio_pub) == 0 || strlen(d_jurisdiccion) == 0 || strlen(d_organismo) == 0 || strlen(d_unidad_fisica) == 0 || strlen(d_observaciones) == 0 || strlen(d_file_name) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    char *file_data_url = get_dynamic_value(body, "file_data");
    if (file_data_url != NULL) {
        char *d_file_data = (char *)malloc(strlen(file_data_url) + 1);
        if (!d_file_data) {
            snprintf(error_msg, error_size, "Error de memoria al decodificar archivo.");
            free(file_data_url);
            return 0;
        }

        url_decode(d_file_data, file_data_url);
        if (!save_upload_file(d_file_path, d_file_data)) {
            snprintf(error_msg, error_size, "No se pudo guardar el archivo físico en el servidor.");
            free(d_file_data); free(file_data_url);
            return 0;
        }
        free(d_file_data); free(file_data_url);
    } else {
        snprintf(error_msg, error_size, "No se recibió el archivo adjunto (file_data).");
        return 0;
    }

    FILE *file_check = fopen(d_file_path, "r");
    if (file_check == NULL) {
        snprintf(error_msg, error_size, "Error: El archivo no se encuentra en el directorio.");
        return 0;
    }
    fclose(file_check);

    if (pListNormasLocal != NULL) {
        for (int i = 0; i < pListNormasLocal->len(pListNormasLocal); i++) {
            Normas* nNorma = (Normas*) pListNormasLocal->get(pListNormasLocal, i);
            if (strcasecmp(nNorma->nombre_norma, d_nombre_norma) == 0 && strcasecmp(nNorma->n_norma, d_n_norma) == 0 && strcasecmp(nNorma->tipo_norma, d_tipo_norma) == 0 && strcasecmp(nNorma->organismo, d_organismo) == 0) {
                snprintf(error_msg, error_size, "Error: Norma Existente.");
                return 0;
            }
        }
    }

    int nuevo_id = sp_insertar_normas(d_nombre_norma, d_n_norma, d_tipo_norma, d_f_norma, d_f_pub, d_anio_pub, d_jurisdiccion, d_organismo, d_unidad_fisica, d_observaciones, d_file_name, d_file_path);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    Normas* nuevaNorma = newNormas();
    if (nuevaNorma) {
        memset(nuevaNorma, 0, sizeof(Normas));
        nuevaNorma->id = nuevo_id;
        strncpy(nuevaNorma->nombre_norma, d_nombre_norma, sizeof(nuevaNorma->nombre_norma) -1);
        strncpy(nuevaNorma->n_norma, d_n_norma, sizeof(nuevaNorma->n_norma) -1);
        strncpy(nuevaNorma->tipo_norma, d_tipo_norma, sizeof(nuevaNorma->tipo_norma) -1);
        strncpy(nuevaNorma->f_norma, d_f_norma, sizeof(nuevaNorma->f_norma) -1);
        strncpy(nuevaNorma->f_pub, d_f_pub, sizeof(nuevaNorma->f_pub) -1);
        strncpy(nuevaNorma->anio_pub, d_anio_pub, sizeof(nuevaNorma->anio_pub) -1);
        strncpy(nuevaNorma->jurisdiccion, d_jurisdiccion, sizeof(nuevaNorma->jurisdiccion) -1);
        strncpy(nuevaNorma->organismo, d_organismo, sizeof(nuevaNorma->organismo) -1);
        strncpy(nuevaNorma->unidad_fisica, d_unidad_fisica, sizeof(nuevaNorma->unidad_fisica) -1);
        strncpy(nuevaNorma->observaciones, d_observaciones, sizeof(nuevaNorma->observaciones) -1);
        strncpy(nuevaNorma->file_name, d_file_name, sizeof(nuevaNorma->file_name) -1);
        strncpy(nuevaNorma->file_path, d_file_path, sizeof(nuevaNorma->file_path) -1);

        pListNormasLocal->add(pListNormasLocal, nuevaNorma);
    }
    return 1;
}

// ===================================================================================================================================== //
// FUNCION EDICIÓN DE REGISTRO (NORMAS)
// ===================================================================================================================================== //
int normas_service_edit(const char *body, char *error_msg, int error_size) {
    // 🔹 CORRECCIÓN CRÍTICA: Se inicializan todos los arrays a ceros {0}
    char id_str[32] = {0};
    char nombre_norma[501] = {0}; char d_nombre_norma[501] = {0};
    char n_norma[101] = {0};      char d_n_norma[101] = {0};
    char tipo_norma[201] = {0};   char d_tipo_norma[201] = {0};
    char f_norma[201] = {0};      char d_f_norma[201] = {0};
    char f_pub[11] = {0};         char d_f_pub[11] = {0};
    char anio_pub[5] = {0};       char d_anio_pub[5] = {0};
    char jurisdiccion[101] = {0}; char d_jurisdiccion[101] = {0};
    char organismo[101] = {0};    char d_organismo[101] = {0};
    char unidad_fisica[101] = {0};char d_unidad_fisica[101] = {0};
    char observaciones[3000] = {0};char d_observaciones[3000] = {0};
    char file_name[101] = {0};    char d_file_name[101] = {0};

    char absolute_file_path[PATH_MAX] = {0};
    char db_file_path[256] = {0};

    get_norma_value(body, "id", id_str, sizeof(id_str));
    int id_a_editar = atoi(id_str);

    get_norma_value(body,"nombre_norma", nombre_norma, sizeof(nombre_norma)); url_decode(d_nombre_norma, nombre_norma);
    get_norma_value(body, "n_norma", n_norma, sizeof(n_norma));             url_decode(d_n_norma, n_norma);
    get_norma_value(body, "tipo_norma", tipo_norma, sizeof(tipo_norma));    url_decode(d_tipo_norma, tipo_norma);
    get_norma_value(body, "f_norma", f_norma, sizeof(f_norma));             url_decode(d_f_norma, f_norma);
    get_norma_value(body, "f_pub", f_pub, sizeof(f_pub));                   url_decode(d_f_pub, f_pub);
    get_norma_value(body, "anio_pub", anio_pub, sizeof(anio_pub));          url_decode(d_anio_pub, anio_pub);
    get_norma_value(body, "jurisdiccion", jurisdiccion, sizeof(jurisdiccion));url_decode(d_jurisdiccion, jurisdiccion);
    get_norma_value(body, "organismo", organismo, sizeof(organismo));       url_decode(d_organismo, organismo);
    get_norma_value(body, "unidad_fisica", unidad_fisica, sizeof(unidad_fisica));url_decode(d_unidad_fisica, unidad_fisica);
    get_norma_value(body, "observaciones", observaciones, sizeof(observaciones));url_decode(d_observaciones, observaciones);
    get_norma_value(body, "file_name", file_name, sizeof(file_name));       url_decode(d_file_name, file_name);

    get_target_upload_disk_path(d_file_name, absolute_file_path, sizeof(absolute_file_path));
    snprintf(db_file_path, sizeof(db_file_path), "../../uploads/%s", d_file_name);

    if (id_a_editar <= 0 || strlen(d_nombre_norma) == 0 || strlen(d_n_norma) == 0 || strlen(d_tipo_norma) == 0 || strlen(d_f_norma) == 0 || strlen(d_organismo) == 0 || strlen(d_file_name) == 0) {
        snprintf(error_msg, error_size, "ID inválido o campos sin completar.");
        return 0;
    }

    char *file_data_url = get_dynamic_value(body, "file_data");
    if (file_data_url != NULL && strlen(file_data_url) > 20) {
        char *d_file_data = (char *)malloc(strlen(file_data_url) + 1);
        if (!d_file_data) {
            snprintf(error_msg, error_size, "Error de memoria al decodificar archivo.");
            free(file_data_url);
            return 0;
        }

        url_decode(d_file_data, file_data_url);
        if (!save_upload_file(absolute_file_path, d_file_data)) {
            snprintf(error_msg, error_size, "No se pudo guardar el archivo físico.");
            free(d_file_data); free(file_data_url);
            return 0;
        }
        free(d_file_data);

        FILE *file_check = fopen(absolute_file_path, "r");
        if (file_check == NULL) {
            snprintf(error_msg, error_size, "Error: El archivo no se encuentra.");
            free(file_data_url);
            return 0;
        }
        fclose(file_check);
    }
    if (file_data_url) free(file_data_url);

    if (pListNormasLocal != NULL) {
        for (int i = 0; i < pListNormasLocal->len(pListNormasLocal); i++) {
            Normas* nNorma = (Normas*) pListNormasLocal->get(pListNormasLocal, i);
            if (nNorma->id != id_a_editar && strcasecmp(nNorma->nombre_norma, d_nombre_norma) == 0 && strcasecmp(nNorma->n_norma, d_n_norma) == 0 && strcasecmp(nNorma->tipo_norma, d_tipo_norma) == 0 && strcasecmp(nNorma->organismo, d_organismo) == 0) {
                snprintf(error_msg, error_size, "Error: Ya existe otra norma clave.");
                return 0;
            }
        }
    }

    if (sp_editar_normas(id_a_editar, d_nombre_norma, d_n_norma, d_tipo_norma, d_f_norma, d_f_pub, d_anio_pub, d_jurisdiccion, d_organismo, d_unidad_fisica, d_observaciones, d_file_name, db_file_path) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    if (pListNormasLocal != NULL) {
        for (int i = 0; i < pListNormasLocal->len(pListNormasLocal); i++) {
            Normas* nNorma = (Normas*) pListNormasLocal->get(pListNormasLocal, i);
            if (nNorma->id == id_a_editar) {
                strncpy(nNorma->nombre_norma, d_nombre_norma, sizeof(nNorma->nombre_norma) - 1);
                strncpy(nNorma->n_norma, d_n_norma, sizeof(nNorma->n_norma) - 1);
                strncpy(nNorma->tipo_norma, d_tipo_norma, sizeof(nNorma->tipo_norma) - 1);
                strncpy(nNorma->f_norma, d_f_norma, sizeof(nNorma->f_norma) - 1);
                strncpy(nNorma->f_pub, d_f_pub, sizeof(nNorma->f_pub) - 1);
                strncpy(nNorma->anio_pub, d_anio_pub, sizeof(nNorma->anio_pub) - 1);
                strncpy(nNorma->jurisdiccion, d_jurisdiccion, sizeof(nNorma->jurisdiccion) - 1);
                strncpy(nNorma->organismo, d_organismo, sizeof(nNorma->organismo) - 1);
                strncpy(nNorma->unidad_fisica, d_unidad_fisica, sizeof(nNorma->unidad_fisica) - 1);
                strncpy(nNorma->observaciones, d_observaciones, sizeof(nNorma->observaciones) - 1);
                strncpy(nNorma->file_name, d_file_name, sizeof(nNorma->file_name) - 1);
                strncpy(nNorma->file_path, db_file_path, sizeof(nNorma->file_path) - 1);
                break;
            }
        }
    }
    return 1;
}

// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_norma_service_id(const char *body, char *json_out, int out_size) {
    char id_str[10] = {0};
    get_norma_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512] = {0};
    snprintf(query, sizeof(query), "SELECT * FROM normas WHERE id = %s LIMIT 1;", id_str);
    DBResult *res = db_query(query);
    MYSQL_ROW row;

    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Norma no encontrada\" }");
        if (res) db_free_result(res);
        return 0;
    }

    char obs_limpias[6500] = {0};
    char nombre_limpio[1200] = {0};
    json_escape_string(row[10] ? row[10] : "", obs_limpias, sizeof(obs_limpias));
    json_escape_string(row[1] ? row[1] : "", nombre_limpio, sizeof(nombre_limpio));

    snprintf(json_out, out_size,
             "{ \"id\": %s, \"nombre_norma\": \"%s\", \"n_norma\": \"%s\", \"tipo_norma\": \"%s\", \"f_norma\": \"%s\", \"f_pub\": \"%s\", \"anio_pub\": \"%s\", \"jurisdiccion\": \"%s\", \"organismo\": \"%s\", \"ubicacion_fisica\": \"%s\", \"observaciones\": \"%s\", \"file_name\": \"%s\", \"file_path\": \"%s\" }",
             row[0] ? row[0] : "0", nombre_limpio, row[2] ? row[2] : "", row[3] ? row[3] : "", row[4] ? row[4] : "", row[5] ? row[5] : "", row[6] ? row[6] : "", row[7] ? row[7] : "", row[8] ? row[8] : "", row[9] ? row[9] : "", obs_limpias, row[11] ? row[11] : "", row[12] ? row[12] : "");

    db_free_result(res);
    return 1;
}

// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //
static void route_post_normas(int client, const char *body) {
    char error_msg[256];
    if (normas_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Norma creada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }
}

static void route_post_normas_edit(int client, const char *body) {
    char error_msg[256];
    if (normas_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Norma actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }
}

static void route_get_normas_list(int client, const char *body) {
    if (pListNormasLocal == NULL) {
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de memoria\"}");
        return;
    }

    // 🔹 CORRECCIÓN: Buffer JSON aumentado a 8192 bytes por registro por precaución extrema
    size_t total_registros = pListNormasLocal->len(pListNormasLocal);
    size_t buffer_size = (total_registros * 8192) + 512;
    char *json = (char*) calloc(1, buffer_size);

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {
        Normas* oneNorma = (Normas*) pListNormasLocal->get(pListNormasLocal, i);

        // 🔹 CORRECCIÓN: Buffers colosales para prevenir cualquier posible truncamiento
        char item[8192] = {0};
        char obs_limpias[6500] = {0};
        char nombre_limpio[1200] = {0};

        json_escape_string(oneNorma->observaciones, obs_limpias, sizeof(obs_limpias));
        json_escape_string(oneNorma->nombre_norma, nombre_limpio, sizeof(nombre_limpio));

        snprintf(item, sizeof(item),
            "{\"id\": %d, \"nombre_norma\": \"%s\", \"n_norma\": \"%s\", \"tipo_norma\": \"%s\", \"f_norma\": \"%s\", \"f_pub\": \"%s\", \"anio_pub\": \"%s\", \"jurisdiccion\": \"%s\", \"organismo\": \"%s\", \"ubicacion_fisica\": \"%s\", \"observaciones\": \"%s\", \"file_name\": \"%s\", \"file_path\": \"%s\", \"descripcion_organismo\": \"%s\" }%s",
            oneNorma->id, nombre_limpio, oneNorma->n_norma, oneNorma->tipo_norma, oneNorma->f_norma, oneNorma->f_pub, oneNorma->anio_pub, oneNorma->jurisdiccion, oneNorma->organismo, oneNorma->unidad_fisica, obs_limpias, oneNorma->file_name, oneNorma->file_path, oneNorma->descripcion_organismo, (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json);
}

static void route_get_norma_by_id(int client, const char *body) {
    // 🔹 CORRECCIÓN: Buffer de respuesta aumentado a 8192
    char response_json[8192] = {0};
    if (get_norma_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }
}


// ========================================================================================================================= //


static void route_post_normas_open_doc(int client, const char *body) {

    char id_str[32] = {0};
    get_norma_value(body, "id", id_str, sizeof(id_str));
    int doc_id = atoi(id_str);

    if (doc_id <= 0) {
        send_response(client, "400 Bad Request", "application/json", "{\"error\": \"ID inválido\"}");
        return;
    }

    // 1. Buscamos el registro
    Normas* targetNorma = NULL;
    if (pListNormasLocal != NULL) {
        for (int i = 0; i < pListNormasLocal->len(pListNormasLocal); i++) {
            Normas* n = (Normas*) pListNormasLocal->get(pListNormasLocal, i);
            if (n->id == doc_id) {
                targetNorma = n;
                break;
            }
        }
    }

    if (targetNorma == NULL) {
        send_response(client, "404 Not Found", "application/json", "{\"error\": \"Norma no encontrada\"}");
        return;
    }

    if (strlen(targetNorma->file_name) == 0) {
        send_response(client, "404 Not Found", "application/json", "{\"error\": \"Esta norma no tiene ningún archivo adjunto\"}");
        return;
    }

    // 2. Construimos la ruta absoluta del archivo físico
    char absolute_file_path[PATH_MAX] = {0};
    get_target_upload_disk_path(targetNorma->file_name, absolute_file_path, sizeof(absolute_file_path));

    // 3. Delegamos todo el trabajo pesado a nuestra nueva librería central
    serve_document(client, absolute_file_path, targetNorma->file_name);
}

// ===================================================================================================================================== //
// INIT ALL ROUTES NORMAS
// ===================================================================================================================================== //
void init_normas_routes() {
    add_route("GET", "/normas/list", route_get_normas_list);
    add_route("POST", "/normas/add", route_post_normas);
    add_route("POST", "/normas/edit", route_post_normas_edit);
    add_route("POST", "/normas/get", route_get_norma_by_id);
    add_route("POST", "/normas/open_doc", route_post_normas_open_doc);
}
