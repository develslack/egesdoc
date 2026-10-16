#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>
#include "../system/hash.h"
#include "../system/routes.h"
#include "../system/db.h"
#include "../system/commonlib.h"
#include "../system/ArrayList.h"
#include "../system/open_docs.h"
#include "normas_vinculadas_service.h"

// Variable estática para la memoria local del módulo
static ArrayList* pListNormasVinculadasLocal = NULL;

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

static void get_target_upload_disk_path(const char *rel_or_name, char *out_disk_path, size_t max_len) {
    char exe_path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", exe_path, PATH_MAX);

    if (count != -1) {
        exe_path[count] = '\0';
        char *exe_dir = dirname(exe_path);

        char temp_dir[PATH_MAX];
        snprintf(temp_dir, sizeof(temp_dir), "%s/../../documentos/%s", exe_dir, rel_or_name);

        char resolved_dir[PATH_MAX];
        if (realpath(temp_dir, resolved_dir) != NULL) {
            snprintf(out_disk_path, max_len, "%s", resolved_dir);
        } else {
            snprintf(out_disk_path, max_len, "%s", temp_dir);
        }
    } else {
        snprintf(out_disk_path, max_len, "../../documentos/%s", rel_or_name);
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
        printf("🚨 [DOCUMENTOS] No se pudo abrir para escritura: %s\n", target_path);
        return 0;
    }

    fwrite(decoded, 1, out_len, f);
    fclose(f);
    free(decoded);

    printf("📄 [DOCUMENTOS] Archivo guardado físicamente en: %s (%zu bytes)\n", target_path, out_len);
    return 1;
}

static int sp_insertar_norma_vinculada(int id_norma_principal, const char* path_name) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[2];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_norma_vinculada(?,?)";

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
    bind_param[0].buffer = (void *)&id_norma_principal;
    bind_param[0].is_unsigned = 0;

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)path_name;
    bind_param[1].buffer_length = strlen(path_name);

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

NormasVinculadas* newNormasVinculadas(){
    NormasVinculadas* nNormaVinculada = (NormasVinculadas*)malloc(sizeof(NormasVinculadas));
    if(nNormaVinculada != NULL){
        memset(nNormaVinculada, 0, sizeof(NormasVinculadas));
    }
    return nNormaVinculada;
}

void normas_vinculadas_init_cache(ArrayList* alistNormasVinculadas) {
    if(alistNormasVinculadas != NULL) {
        pListNormasVinculadasLocal = alistNormasVinculadas;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ NORMAS VINCULADAS ].\n");
    } else {
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ NORMAS VINCULADAS ] con NULL.\n");
    }
}

void normas_vinculadas_load_storage(ArrayList* alistNormasVinculadas) {
    if(alistNormasVinculadas == NULL) return;

    DBResult *res = db_query("SELECT * FROM normas_vinculadas");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        NormasVinculadas* nNormaVinculada = newNormasVinculadas();
        if (nNormaVinculada != NULL) {
            nNormaVinculada->id = atoi(row[0]);
            nNormaVinculada->id_norma_principal = atoi(row[1]);
            strncpy(nNormaVinculada->path_name, row[2] ? row[2] : "", sizeof(nNormaVinculada->path_name) - 1);

            alistNormasVinculadas->add(alistNormasVinculadas, nNormaVinculada);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d NORMA VINCULADAS cargadas. Espacio reservado: %d slots.\n", alistNormasVinculadas->len(alistNormasVinculadas), alistNormasVinculadas->reservedSize);
}

static void get_norma_vinculada_value(const char *body, const char *key, char *out, size_t out_size) {
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

static void json_escape_string_extendido(const char *src, char *dest, size_t max_len) {
    size_t j = 0;
    for (size_t i = 0; src[i] != '\0' && j < max_len - 2; i++) {
        switch (src[i]) {
            case '\"': dest[j++] = '\\'; dest[j++] = '\"'; break;
            case '\\': dest[j++] = '\\'; dest[j++] = '\\'; break;
            case '\n': dest[j++] = '\\'; dest[j++] = 'n'; break;
            case '\r': dest[j++] = '\\'; dest[j++] = 'r'; break;
            case '\t': dest[j++] = '\\'; dest[j++] = 't'; break;
            default:
                if ((unsigned char)src[i] >= '\x20') dest[j++] = src[i];
                break;
        }
    }
    dest[j] = '\0';
}

int get_informacion_extendida_service(const char *body, char *json_out, int out_size) {
    char id_str[16] = {0};
    get_norma_vinculada_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT n.id, n.nombre_norma, n.n_norma, n.tipo_norma, n.f_norma, "
             "n.f_pub, n.anio_pub, n.jurisdiccion, n.organismo, n.unidad_fisica, "
             "n.observaciones, n.file_name, n.file_path, "
             "o.saf, o.descripcion AS org_desc, j.descripcion AS jur_desc "
             "FROM normas n "
             "LEFT JOIN organismos o ON n.organismo = o.cod_org "
             "LEFT JOIN jurisdicciones j ON n.jurisdiccion = j.cod_jur "
             "WHERE n.id = %s LIMIT 1;", id_str);

    DBResult *res = db_query(query);
    MYSQL_ROW row;

    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Información extendida no encontrada en la base de datos.\" }");
        if (res) db_free_result(res);
        return 0;
    }

    char obs_limpias[3500] = {0};
    char nombre_limpio[600] = {0};
    char org_desc_limpia[600] = {0};
    char jur_desc_limpia[600] = {0};

    json_escape_string_extendido(row[10] ? row[10] : "", obs_limpias, sizeof(obs_limpias));
    json_escape_string_extendido(row[1] ? row[1] : "", nombre_limpio, sizeof(nombre_limpio));
    json_escape_string_extendido(row[14] ? row[14] : "", org_desc_limpia, sizeof(org_desc_limpia));
    json_escape_string_extendido(row[15] ? row[15] : "", jur_desc_limpia, sizeof(jur_desc_limpia));

    snprintf(json_out, out_size,
             "{ \"id\": %s, \"nombre_norma\": \"%s\", \"n_norma\": \"%s\", \"tipo_norma\": \"%s\", \"f_norma\": \"%s\", "
             "\"f_pub\": \"%s\", \"anio_pub\": \"%s\", \"jurisdiccion_cod\": \"%s\", \"organismo_cod\": \"%s\", "
             "\"ubicacion_fisica\": \"%s\", \"observaciones\": \"%s\", \"file_name\": \"%s\", \"file_path\": \"%s\", "
             "\"saf\": \"%s\", \"org_desc\": \"%s\", \"jur_desc\": \"%s\" }",
             row[0] ? row[0] : "0",
             nombre_limpio,
             row[2] ? row[2] : "",
             row[3] ? row[3] : "",
             row[4] ? row[4] : "",
             row[5] ? row[5] : "",
             row[6] ? row[6] : "",
             row[7] ? row[7] : "",
             row[8] ? row[8] : "",
             row[9] ? row[9] : "",
             obs_limpias,
             row[11] ? row[11] : "",
             row[12] ? row[12] : "",
             row[13] ? row[13] : "",
             org_desc_limpia,
             jur_desc_limpia);

    db_free_result(res);
    return 1;
}

int normas_vinculadas_service_register(const char *body, char *error_msg, int error_size) {
    char id_norma_principal[11] = {0};
    char file_name[101] = {0};
    char d_file_name[101] = {0};
    char carpeta_norma[150] = {0};
    char d_carpeta_norma[150] = {0};
    char target_dir[PATH_MAX] = {0};
    char target_file_path[PATH_MAX] = {0};
    char db_path[PATH_MAX] = {0};

    get_norma_vinculada_value(body, "id_norma_principal", id_norma_principal, sizeof(id_norma_principal));
    int id_norma = atoi(id_norma_principal);

    get_norma_vinculada_value(body, "carpeta_norma", carpeta_norma, sizeof(carpeta_norma));
    url_decode(d_carpeta_norma, carpeta_norma);

    get_norma_vinculada_value(body, "file_name", file_name, sizeof(file_name));
    url_decode(d_file_name, file_name);

    if (id_norma <= 0 || strlen(d_file_name) == 0) {
        snprintf(error_msg, error_size, "ID o nombre de archivo inválidos.");
        return 0;
    }

    if (strlen(d_carpeta_norma) == 0) {
        snprintf(d_carpeta_norma, sizeof(d_carpeta_norma), "norma_%d", id_norma);
    }

    // Resolver ruta absoluta del directorio destino en documentos/
    char rel_dir[256];
    snprintf(rel_dir, sizeof(rel_dir), "%s", d_carpeta_norma);
    get_target_upload_disk_path(rel_dir, target_dir, sizeof(target_dir));

    // Crear la carpeta física si aún no existe
    struct stat st = {0};
    if (stat(target_dir, &st) == -1) {
        mkdir(target_dir, 0755);
    }

    snprintf(target_file_path, sizeof(target_file_path), "%s/%s", target_dir, d_file_name);
    snprintf(db_path, sizeof(db_path), "../../documentos/%s/%s", d_carpeta_norma, d_file_name);

    char *file_data_url = get_dynamic_value(body, "file_data");
    if (file_data_url != NULL) {
        char *d_file_data = (char *)malloc(strlen(file_data_url) + 1);
        if (!d_file_data) {
            snprintf(error_msg, error_size, "Error de memoria al decodificar archivo.");
            free(file_data_url);
            return 0;
        }

        url_decode(d_file_data, file_data_url);
        if (!save_upload_file(target_file_path, d_file_data)) {
            snprintf(error_msg, error_size, "No se pudo guardar el archivo físico en el servidor.");
            free(d_file_data);
            free(file_data_url);
            return 0;
        }
        free(d_file_data);
        free(file_data_url);
    } else {
        snprintf(error_msg, error_size, "No se recibió el archivo adjunto (file_data).");
        return 0;
    }

    FILE *file_check = fopen(target_file_path, "r");
    if (file_check == NULL) {
        snprintf(error_msg, error_size, "Error: El archivo no se encuentra en el directorio.");
        return 0;
    }
    fclose(file_check);

    int nuevo_id = sp_insertar_norma_vinculada(id_norma, db_path);
    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    NormasVinculadas* nuevaNormaVinculada = newNormasVinculadas();
    if (nuevaNormaVinculada) {
        memset(nuevaNormaVinculada, 0, sizeof(NormasVinculadas));
        nuevaNormaVinculada->id = nuevo_id;
        nuevaNormaVinculada->id_norma_principal = id_norma;
        strncpy(nuevaNormaVinculada->path_name, db_path, sizeof(nuevaNormaVinculada->path_name) - 1);
        pListNormasVinculadasLocal->add(pListNormasVinculadasLocal, nuevaNormaVinculada);
    }
    return 1;
}

// ===================================================================================================================================== //
// HANDLERS
// ===================================================================================================================================== //
static void route_post_norma_vinculada(int client, const char *body) {
    char error_msg[256];
    if (normas_vinculadas_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Norma Vinculada cargada correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }
}

static void route_get_normas_vinculadas_list(int client, const char *body) {
    if (pListNormasVinculadasLocal == NULL) {
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    size_t total_registros = pListNormasVinculadasLocal->len(pListNormasVinculadasLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size);

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");
    for (int i = 0; i < total_registros; i++) {
        NormasVinculadas* oneNormaVinculada = (NormasVinculadas*) pListNormasVinculadasLocal->get(pListNormasVinculadasLocal, i);
        char item[600];
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"id_norma_principal\": %d, \"path_name\": \"%s\" }%s",
            oneNormaVinculada->id, oneNormaVinculada->id_norma_principal, oneNormaVinculada->path_name, (i < total_registros - 1) ? "," : "");
        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json);
}

static void route_post_informacion_extendida(int client, const char *body) {
    char response_json[8192] = {0};
    if (get_informacion_extendida_service(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }
}

// Apertura física de un archivo vinculado a través de serve_document
static void route_post_normas_vinculadas_open_doc(int client, const char *body) {
    char id_str[32] = {0};
    char file_name_param[128] = {0};
    char d_file_name_param[128] = {0};

    get_norma_vinculada_value(body, "id", id_str, sizeof(id_str));
    get_norma_vinculada_value(body, "file_name", file_name_param, sizeof(file_name_param));
    url_decode(d_file_name_param, file_name_param);

    int doc_id = atoi(id_str);
    if (doc_id <= 0) {
        send_response(client, "400 Bad Request", "application/json", "{\"error\": \"ID de norma vinculada inválido\"}");
        return;
    }

    NormasVinculadas* target = NULL;
    if (pListNormasVinculadasLocal != NULL) {
        for (int i = 0; i < pListNormasVinculadasLocal->len(pListNormasVinculadasLocal); i++) {
            NormasVinculadas* nv = (NormasVinculadas*) pListNormasVinculadasLocal->get(pListNormasVinculadasLocal, i);
            if (nv->id == doc_id) {
                target = nv;
                break;
            }
        }
    }

    if (target == NULL) {
        send_response(client, "404 Not Found", "application/json", "{\"error\": \"Registro de norma vinculada no encontrado\"}");
        return;
    }

    char absolute_path[PATH_MAX] = {0};
    char exe_path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", exe_path, PATH_MAX);

    if (count != -1) {
        exe_path[count] = '\0';
        char *exe_dir = dirname(exe_path);

        if (strlen(d_file_name_param) > 0) {
            snprintf(absolute_path, sizeof(absolute_path), "%s/%s/%s", exe_dir, target->path_name, d_file_name_param);
        } else {
            snprintf(absolute_path, sizeof(absolute_path), "%s/%s", exe_dir, target->path_name);
        }
    } else {
        snprintf(absolute_path, sizeof(absolute_path), "%s", target->path_name);
    }

    char resolved[PATH_MAX];
    if (realpath(absolute_path, resolved) != NULL) {
        strncpy(absolute_path, resolved, sizeof(absolute_path) - 1);
    }

    const char *nombre_archivo_final = strlen(d_file_name_param) > 0 ? d_file_name_param : basename(absolute_path);
    serve_document(client, absolute_path, nombre_archivo_final);
}

// Itera el directorio asignado en path_name y devuelve la lista de archivos reales en disco
static void route_post_normas_vinculadas_listar_archivos(int client, const char *body) {
    char id_str[32] = {0};
    get_norma_vinculada_value(body, "id", id_str, sizeof(id_str));
    int id_nv = atoi(id_str);

    NormasVinculadas* target = NULL;
    if (pListNormasVinculadasLocal != NULL) {
        for (int i = 0; i < pListNormasVinculadasLocal->len(pListNormasVinculadasLocal); i++) {
            NormasVinculadas* nv = (NormasVinculadas*) pListNormasVinculadasLocal->get(pListNormasVinculadasLocal, i);
            if (nv->id == id_nv) {
                target = nv;
                break;
            }
        }
    }

    if (!target) {
        send_response(client, "404 Not Found", "application/json", "{\"error\": \"Norma vinculada no encontrada\"}");
        return;
    }

    char exe_path[PATH_MAX];
    char dir_path[PATH_MAX] = {0};
    ssize_t count = readlink("/proc/self/exe", exe_path, PATH_MAX);
    if (count != -1) {
        exe_path[count] = '\0';
        char *exe_dir = dirname(exe_path);
        snprintf(dir_path, sizeof(dir_path), "%s/%s", exe_dir, target->path_name);
    } else {
        snprintf(dir_path, sizeof(dir_path), "%s", target->path_name);
    }

    char resolved[PATH_MAX];
    if (realpath(dir_path, resolved) != NULL) {
        strncpy(dir_path, resolved, sizeof(dir_path) - 1);
    }

    // Si path_name apunta directamente a un archivo único
    struct stat s;
    if (stat(dir_path, &s) == 0 && S_ISREG(s.st_mode)) {
        char single_json[512];
        snprintf(single_json, sizeof(single_json), "[\"%s\"]", basename(dir_path));
        send_response(client, "200 OK", "application/json", single_json);
        return;
    }

    DIR *d = opendir(dir_path);
    if (!d) {
        send_response(client, "200 OK", "application/json", "[]");
        return;
    }

    char *json = (char*) calloc(1, 4096);
    strcpy(json, "[");
    struct dirent *dir;
    int first = 1;

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
        if (!first) strcat(json, ",");
        first = 0;
        strcat(json, "\"");
        strcat(json, dir->d_name);
        strcat(json, "\"");
    }
    closedir(d);
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json);
}

void init_normas_vinculadas_routes() {
    add_route("GET", "/normas_vinculadas/list", route_get_normas_vinculadas_list);
    add_route("POST", "/normas_vinculadas/add", route_post_norma_vinculada);
    add_route("POST", "/normas_vinculadas/extendida", route_post_informacion_extendida);
    add_route("POST", "/normas_vinculadas/open_doc", route_post_normas_vinculadas_open_doc);
    add_route("POST", "/normas_vinculadas/archivos", route_post_normas_vinculadas_listar_archivos);
}
