#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>
#include <math.h>
#include "../system/hash.h"
#include "../system/routes.h"
#include "../system/db.h"
#include "../system/commonlib.h"
#include "../system/ArrayList.h"
#include "representantes_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListRepresentantesLocal = NULL;

// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (REPRESENTANTES)
// ===================================================================================================================================== //
static int sp_insertar_representante(const char* nombre_representante, const char* dni_representante, const char* email_representante) {

    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[3];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_representante(?,?,?)";

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

    bind_param[0].buffer_type = MYSQL_TYPE_STRING;
    bind_param[0].buffer = (char *)nombre_representante;
    bind_param[0].buffer_length = strlen(nombre_representante);

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)dni_representante;
    bind_param[1].buffer_length = strlen(dni_representante);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)email_representante;
    bind_param[2].buffer_length = strlen(email_representante);


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

} // END OF FUNCTION


// ===================================================================================================================================== //


static int sp_editar_representante(int id, const char* nombre_representante, const char* dni_representante, const char* email_representante) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[4];
    const char *query = "CALL sp_editar_representante(?,?,?,?)";

    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) {
        if (stmt) mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    // PONEMOS A CERO EL OBJETO
    memset(bind_param, 0, sizeof(bind_param));

    bind_param[0].buffer_type = MYSQL_TYPE_LONG;
    bind_param[0].buffer = (void *)&id;
    bind_param[0].is_unsigned = 0;

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)nombre_representante;
    bind_param[1].buffer_length = strlen(nombre_representante);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)dni_representante;
    bind_param[2].buffer_length = strlen(dni_representante);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)email_representante;
    bind_param[3].buffer_length = strlen(email_representante);


    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    mysql_close(conn);
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //


// ===================================================================================================================================== //
// CONSTRUCTOR
// ===================================================================================================================================== //
Representantes* newRepresentante(){

    Representantes* nRepresentante = (Representantes*)malloc(sizeof(Representantes));

    if(nRepresentante != NULL){
        memset(nRepresentante, 0, sizeof(Representantes));
    }

    return nRepresentante;

} // END OF FUNCTION


// ===================================================================================================================================== //


// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void representantes_init_cache(ArrayList* alistRepresentantes) {

    if(alistRepresentantes != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListRepresentantesLocal = alistRepresentantes;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ REPRESENTANTES ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ REPRESENTANTES ] con NULL.\n");
    }
} // END OF FUNCTION


// ===================================================================================================================================== //


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void representantes_load_storage(ArrayList* alistRepresentantes) {

    if(alistRepresentantes == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT * FROM representantes");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        Representantes* nRepresentante = newRepresentante();

        if (nRepresentante != NULL) {
            nRepresentante->id = atoi(row[0]);
            strncpy(nRepresentante->nombre_representante, row[1] ? row[1] : "", sizeof(nRepresentante->nombre_representante) -1);
            strncpy(nRepresentante->dni_representante, row[2] ? row[2] : "", sizeof(nRepresentante->dni_representante) -1);
            strncpy(nRepresentante->email_representante, row[3] ? row[3] : "", sizeof(nRepresentante->email_representante) -1);

            alistRepresentantes->add(alistRepresentantes, nRepresentante);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d REPRESENTANTES cargados. Espacio reservado: %d slots.\n", alistRepresentantes->len(alistRepresentantes), alistRepresentantes->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
static void get_representantes_value(const char *body, const char *key, char *out, size_t out_size) {

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
// función auxiliar: Escapa caracteres especiales para generar un JSON válido
// ===================================================================================================================================== //
static void json_escape_string(const char *src, char *dest, size_t max_len) {
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
// FUNCION PARA INSERTAR NUEVO REGISTRO
// ===================================================================================================================================== //
int representantes_service_register(const char *body, char *error_msg, int error_size) {

    // CORRECCIÓN: Inicialización con {0}
    char nombre_representante[101] = {0}, d_nombre_representante[101] = {0};
    char dni_representante[11] = {0}, d_dni_representante[11] = {0};
    char email_representante[51] = {0}, d_email_representante[51] = {0};

    // CAPTURAMOS LOS DATOS
    get_representantes_value(body, "nombre_representante", nombre_representante, sizeof(nombre_representante));
    url_decode(d_nombre_representante, nombre_representante);

    get_representantes_value(body, "dni_representante", dni_representante, sizeof(dni_representante));
    url_decode(d_dni_representante, dni_representante);

    get_representantes_value(body, "email_representante", email_representante, sizeof(email_representante));
    url_decode(d_email_representante, email_representante);

    // EVALUAMOS SI LLEGA ALGÚN CAMPO VACIO
    if (strlen(d_nombre_representante) == 0 || strlen(d_dni_representante) == 0 || strlen(d_email_representante) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListRepresentantesLocal != NULL) {

        for (int i = 0; i < pListRepresentantesLocal->len(pListRepresentantesLocal); i++) {

            Representantes* nRepresentante = (Representantes*) pListRepresentantesLocal->get(pListRepresentantesLocal, i);

            if (strcasecmp(nRepresentante->nombre_representante, d_nombre_representante) == 0 && strcasecmp(nRepresentante->dni_representante, d_dni_representante) == 0 && strcasecmp(nRepresentante->email_representante, d_email_representante) == 0) {

                snprintf(error_msg, error_size, "Error: Registro existente.");
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS

    int nuevo_id = sp_insertar_representante(d_nombre_representante, d_dni_representante, d_email_representante);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    Representantes* nuevoRepresentante = newRepresentante();

    if (nuevoRepresentante) {

        // Aprovechamos para inicializar el bloque de memoria limpio
        memset(nuevoRepresentante, 0, sizeof(Representantes));

        nuevoRepresentante->id = nuevo_id;
        strncpy(nuevoRepresentante->nombre_representante, d_nombre_representante, sizeof(nuevoRepresentante->nombre_representante) -1);
        strncpy(nuevoRepresentante->dni_representante, d_dni_representante, sizeof(nuevoRepresentante->dni_representante) -1);
        strncpy(nuevoRepresentante->email_representante, d_email_representante, sizeof(nuevoRepresentante->email_representante) -1);

        // Sincronizamos el ArrayList inmediatamente
        pListRepresentantesLocal->add(pListRepresentantesLocal, nuevoRepresentante);

        printf("✅ Sincronización exitosa: REPRESENTANTE [%s] (ID: %d) añadido a RAM.\n", d_nombre_representante, nuevo_id);
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// FUNCION EDICIÓN DE REGISTRO
// ===================================================================================================================================== //
int representantes_service_edit(const char *body, char *error_msg, int error_size) {

    // CORRECCIÓN: Inicialización con {0}
    char id_str[32] = {0};
    char nombre_representante[101] = {0}, d_nombre_representante[101] = {0};
    char dni_representante[11] = {0}, d_dni_representante[11] = {0};
    char email_representante[51] = {0}, d_email_representante[51] = {0};

    get_representantes_value(body, "id", id_str, sizeof(id_str));

    get_representantes_value(body, "nombre_representante", nombre_representante, sizeof(nombre_representante));
    url_decode(d_nombre_representante, nombre_representante);

    get_representantes_value(body, "dni_representante", dni_representante, sizeof(dni_representante));
    url_decode(d_dni_representante, dni_representante);

    get_representantes_value(body, "email_representante", email_representante, sizeof(email_representante));
    url_decode(d_email_representante, email_representante);

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_nombre_representante) == 0 || strlen(d_dni_representante) == 0 || strlen(d_email_representante) == 0){
        snprintf(error_msg, error_size, "ID, o Alguno de los otros campos no contienen datos");
        return 0;
    }

    // 2. VERIFICACIÓN DE EXISTENCIA Y DUPLICADOS EN MEMORIA
    if (pListRepresentantesLocal != NULL) {
        for (int i = 0; i < pListRepresentantesLocal->len(pListRepresentantesLocal); i++) {
            Representantes* nRepresentante = (Representantes*) pListRepresentantesLocal->get(pListRepresentantesLocal, i);

            // Si el nombre ya existe en otro ID, rebotamos la edición
            if (nRepresentante->id != id_a_editar
                    && strcasecmp(nRepresentante->nombre_representante, d_nombre_representante) == 0
                    && strcasecmp(nRepresentante->dni_representante, d_dni_representante) == 0
                    && strcasecmp(nRepresentante->email_representante, d_email_representante) == 0) {
                snprintf(error_msg, error_size, "Error: Registro Existente.");
                return 0;
            }
        }
    }

    // 3. ACTUALIZAR EN BASE DE DATOS (Blindado)
    if (sp_editar_representante(id_a_editar, d_nombre_representante, d_dni_representante, d_email_representante) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 4. ACTUALIZAR EN MEMORIA (ArrayList)
    if (pListRepresentantesLocal != NULL) {
        for (int i = 0; i < pListRepresentantesLocal->len(pListRepresentantesLocal); i++) {
            Representantes* nRepresentante = (Representantes*) pListRepresentantesLocal->get(pListRepresentantesLocal, i);

            if (nRepresentante->id == id_a_editar) {
                // Actualizamos el puntero directamente en la memoria
                strncpy(nRepresentante->nombre_representante, d_nombre_representante, sizeof(nRepresentante->nombre_representante) -1);
                strncpy(nRepresentante->dni_representante, d_dni_representante, sizeof(nRepresentante->dni_representante) -1);
                strncpy(nRepresentante->email_representante, d_email_representante, sizeof(nRepresentante->email_representante) -1);
                printf("✅ Memoria sincronizada: REPRESENTANTE ID %d actualizado.\n", id_a_editar);
                break;
            }
        }
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_representante_service_id(const char *body, char *json_out, int out_size) {

    char id_str[10] = {0};
    get_representantes_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512] = {0};

    snprintf(query, sizeof(query),
             "SELECT * FROM representantes WHERE id = %s LIMIT 1;", id_str);

    DBResult *res = db_query(query);

    MYSQL_ROW row;

    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Registro no encontrado\" }");
        if (res) db_free_result(res);
        return 0;
    }

    char nombre_limpio[300] = {0};
    json_escape_string(row[1] ? row[1] : "", nombre_limpio, sizeof(nombre_limpio));

    // Construimos el JSON usando los índices del array 'row'
    snprintf(json_out, out_size,
             "{ \"id\": %s, \"nombre_representante\": \"%s\", \"dni_representante\": \"%s\", \"email_representante\": \"%s\"}",
             row[0] ? row[0] : "0",
             nombre_limpio,
             row[2] ? row[2] : "",
             row[3] ? row[3] : "");

    db_free_result(res);
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_representante(int client, const char *body) {

    char error_msg[256];

    if (representantes_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Representante creado y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// Handler POST para la edición
// ===================================================================================================================================== //
static void route_post_representante_edit(int client, const char *body) {

    char error_msg[256];

    if (representantes_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Representante actualizado correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES FOR LIST
// ===================================================================================================================================== //
static void route_get_representantes_list(int client, const char *body) {


    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListRepresentantesLocal == NULL) {
        printf("❌ Error crítico: pListRepresentantesLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    size_t total_registros = pListRepresentantesLocal->len(pListRepresentantesLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {

        Representantes* oneRepresentante = (Representantes*) pListRepresentantesLocal->get(pListRepresentantesLocal, i);

        char item[600] = {0};
        char nombre_limpio[300] = {0};

        json_escape_string(oneRepresentante->nombre_representante, nombre_limpio, sizeof(nombre_limpio));

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"nombre_representante\": \"%s\", \"dni_representante\": \"%s\", \"email_representante\": \"%s\"  }%s",
            oneRepresentante->id, nombre_limpio, oneRepresentante->dni_representante, oneRepresentante->email_representante,  (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON


} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTE OR GET ONE REGESTRY
// ===================================================================================================================================== //
static void route_get_representante_by_id(int client, const char *body) {

    char response_json[1024] = {0};

    if (get_representante_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }

} //END OF FUNCTION


// ===================================================================================================================================== //
// INIT ALL ROUTES
// ===================================================================================================================================== //
void init_representantes_routes() {

    add_route("GET", "/representantes/list", route_get_representantes_list); // endpoint para listar
    add_route("POST", "/representantes/add", route_post_representante); // endpoint para alta de nuevo registro
    add_route("POST", "/representantes/edit", route_post_representante_edit); // endpoint para editar un registro
    add_route("POST", "/representantes/get", route_get_representante_by_id); // endpoint para consultar un registro por ID
}
