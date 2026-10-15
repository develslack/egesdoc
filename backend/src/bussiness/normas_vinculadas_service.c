#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>
#include <math.h>
#include "../system/hash.h"
#include "../system/routes.h"
#include "../system/db.h"
#include "../system/commonlib.h"
#include "../system/ArrayList.h"
#include "normas_vinculadas_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListNormasVinculadasLocal = NULL;


// ===================================================================================================================================== //
// Codificador y Decodificador Base64 (Universal)
// ===================================================================================================================================== //

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// ===================================================================================================================================== //

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


// ===================================================================================================================================== //


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
        snprintf(temp_dir, sizeof(temp_dir), "%s/../../documentos", exe_dir);

        char resolved_dir[PATH_MAX];
        if (realpath(temp_dir, resolved_dir) != NULL) {
            snprintf(out_disk_path, max_len, "%s/%s", resolved_dir, filename);
        } else {
            snprintf(out_disk_path, max_len, "%s/%s", temp_dir, filename);
        }
    } else {
        snprintf(out_disk_path, max_len, "../../documentos/%s", filename);
    }
}


// ===================================================================================================================================== //


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
// 🔒 STORED PROCEDURES: Prepared Statements (NORMAS VINCULADAS)
// ===================================================================================================================================== //
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

    // PONEMOS EN CERO EL OBJETO COMPLETO
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


// ===================================================================================================================================== //


// ===================================================================================================================================== //
// CONSTRUCTOR
// ===================================================================================================================================== //
NormasVinculadas* newNormasVinculadas(){

    NormasVinculadas* nNormaVinculada = (NormasVinculadas*)malloc(sizeof(NormasVinculadas));

    if(nNormaVinculada != NULL){
        memset(nNormaVinculada, 0, sizeof(NormasVinculadas));
    }

    return nNormaVinculada;

} // END OF FUNCTION


// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void normas_vinculadas_init_cache(ArrayList* alistNormasVinculadas) {

    if(alistNormasVinculadas != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListNormasVinculadasLocal = alistNormasVinculadas;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ NORMAS VINCULADAS ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ NORMAS VINCULADAS ] con NULL.\n");
    }
} // END OF FUNCTION



// ===================================================================================================================================== //


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void normas_vinculadas_load_storage(ArrayList* alistNormasVinculadas) {

    if(alistNormasVinculadas == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT * FROM normas_vinculadas");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        NormasVinculadas* nNormaVinculada = newNormasVinculadas();

        if (nNormaVinculada != NULL) {
            nNormaVinculada->id = atoi(row[0]);
            nNormaVinculada->id_norma_principal = atoi(row[1]);
            strncpy(nNormaVinculada->path_name, row[2], sizeof(nNormaVinculada->path_name) -1);

            alistNormasVinculadas->add(alistNormasVinculadas, nNormaVinculada);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d NORMA VINCULADAS cargadas. Espacio reservado: %d slots.\n", alistNormasVinculadas->len(alistNormasVinculadas), alistNormasVinculadas->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
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

} // END OF FUNCTION


// ===================================================================================================================================== //


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


// ===================================================================================================================================== //
// función auxiliar: Escapa caracteres especiales para el JSON extendido
// ===================================================================================================================================== //
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

// ===================================================================================================================================== //
// LOGICA DE CONSULTA: INFORMACIÓN EXTENDIDA (SQL UNIFICADO CON LEFT JOIN)
// ===================================================================================================================================== //
int get_informacion_extendida_service(const char *body, char *json_out, int out_size) {
    char id_str[16] = {0};
    // Extraemos el ID de la norma que llega en el body
    get_norma_vinculada_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[1024];
    // Ejecutamos el SQL unificado que reemplaza las múltiples consultas PHP
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

    // Buffers limpios para evitar caracteres de control en el JSON
    char obs_limpias[3500] = {0};
    char nombre_limpio[600] = {0};
    char org_desc_limpia[600] = {0};
    char jur_desc_limpia[600] = {0};

    json_escape_string_extendido(row[10] ? row[10] : "", obs_limpias, sizeof(obs_limpias));
    json_escape_string_extendido(row[1] ? row[1] : "", nombre_limpio, sizeof(nombre_limpio));
    json_escape_string_extendido(row[14] ? row[14] : "", org_desc_limpia, sizeof(org_desc_limpia));
    json_escape_string_extendido(row[15] ? row[15] : "", jur_desc_limpia, sizeof(jur_desc_limpia));

    // Construcción del objeto JSON de respuesta
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


// ===================================================================================================================================== //
// FUNCION PARA REGISTRAR NUEVA NORMA
// ===================================================================================================================================== //
int normas_vinculadas_service_register(const char *body, char *error_msg, int error_size) {
    // 🔹 CORRECCIÓN CRÍTICA: Se inicializan todos los arrays a ceros {0} para evitar arrastrar basura
    char id_norma_principal[11];
    char file_name[101] = {0};    char d_file_name[101] = {0};
    char d_name_path[PATH_MAX] = {0};

    get_norma_vinculada_value(body, "id_norma_principal", id_norma_principal, sizeof(id_norma_principal));

    int id_norma = atoi(id_norma_principal);

    get_norma_vinculada_value(body, "file_name", file_name, sizeof(file_name));       url_decode(d_file_name, file_name);

    get_target_upload_disk_path(d_file_name, d_name_path, sizeof(d_name_path));

    if (strlen(d_file_name) == 0){
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
        if (!save_upload_file(d_name_path, d_file_data)) {
            snprintf(error_msg, error_size, "No se pudo guardar el archivo físico en el servidor.");
            free(d_file_data); free(file_data_url);
            return 0;
        }
        free(d_file_data); free(file_data_url);
    } else {
        snprintf(error_msg, error_size, "No se recibió el archivo adjunto (file_data).");
        return 0;
    }

    FILE *file_check = fopen(d_name_path, "r");
    if (file_check == NULL) {
        snprintf(error_msg, error_size, "Error: El archivo no se encuentra en el directorio.");
        return 0;
    }
    fclose(file_check);

    if (pListNormasVinculadasLocal != NULL) {
        for (int i = 0; i < pListNormasVinculadasLocal->len(pListNormasVinculadasLocal); i++) {
            NormasVinculadas* nNormaVinculada = (NormasVinculadas*) pListNormasVinculadasLocal->get(pListNormasVinculadasLocal, i);
            if (strcasecmp(nNormaVinculada->path_name, d_name_path) == 0) {
                snprintf(error_msg, error_size, "Error: Norma Existente.");
                return 0;
            }
        }
    }

    int nuevo_id = sp_insertar_norma_vinculada(id_norma, d_name_path);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    NormasVinculadas* nuevaNormaVinculada = newNormasVinculadas();
    if (nuevaNormaVinculada) {
        memset(nuevaNormaVinculada, 0, sizeof(NormasVinculadas));
        nuevaNormaVinculada->id = nuevo_id;
        nuevaNormaVinculada->id_norma_principal = id_norma;
        strncpy(nuevaNormaVinculada->path_name, d_name_path, sizeof(nuevaNormaVinculada->path_name) -1);

        pListNormasVinculadasLocal->add(pListNormasVinculadasLocal, nuevaNormaVinculada);
    }
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_norma_vinculada(int client, const char *body) {

    char error_msg[256];

    if (normas_vinculadas_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Norma Vinculada creada y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION



// ===================================================================================================================================== //
// ROUTES FOR LIST
// ===================================================================================================================================== //
static void route_get_normas_vinculadas_list(int client, const char *body) {


    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListNormasVinculadasLocal == NULL) {
        printf("❌ Error crítico: pListNormasVinculadasLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    // Estimamos el tamaño del JSON (aprox 150 bytes por médico)
    size_t total_registros = pListNormasVinculadasLocal->len(pListNormasVinculadasLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {

        NormasVinculadas* oneNormaVinculada = (NormasVinculadas*) pListNormasVinculadasLocal->get(pListNormasVinculadasLocal, i);

        char item[600];

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"id_norma_principal\": %d, \"path_name\": \"%s\"  }%s",
            oneNormaVinculada->id, oneNormaVinculada->id_norma_principal, oneNormaVinculada->path_name,  (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON


} // END OF FUNCTION


// ===================================================================================================================================== //


static void route_post_informacion_extendida(int client, const char *body) {
    char response_json[8192] = {0};
    if (get_informacion_extendida_service(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }
}




// ===================================================================================================================================== //
// INIT ALL ROUTES
// ===================================================================================================================================== //
void init_normas_vinculadas_routes() {

    add_route("GET", "/normas_vinculadas/list", route_get_normas_vinculadas_list); // endpoint para listar
    add_route("POST", "/normas_vinculadas/add", route_post_norma_vinculada); // endpoint para alta de nuevo registro
    add_route("POST", "/normas_vinculadas/extendida", route_post_informacion_extendida);

} // END OF FUNCTION
