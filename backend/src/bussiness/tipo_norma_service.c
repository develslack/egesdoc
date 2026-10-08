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
#include "tipo_norma_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListTipoNormaLocal = NULL;

// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (TIPO ORGANISMO)
// ===================================================================================================================================== //
static int sp_insertar_tipo_norma(const char* descripcion) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[1];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_tipo_norma(?)";

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
    bind_param[0].buffer = (char *)descripcion;
    bind_param[0].buffer_length = strlen(descripcion);

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


static int sp_editar_tipo_norma(int id, const char* descripcion) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[2];
    const char *query = "CALL sp_editar_tipo_norma(?, ?)";

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
    bind_param[1].buffer = (char *)descripcion;
    bind_param[1].buffer_length = strlen(descripcion);


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
TipoNorma* newTipoNorma(){

    TipoNorma* nTipoNorma = (TipoNorma*)malloc(sizeof(TipoNorma));

    if(nTipoNorma != NULL){
        memset(nTipoNorma, 0, sizeof(TipoNorma));
    }

    return nTipoNorma;

} // END OF FUNCTION


// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void tipo_norma_init_cache(ArrayList* alistTipoNorma) {

    if(alistTipoNorma != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListTipoNormaLocal = alistTipoNorma;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ TIPO NORMA ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ TIPO NORMA ] con NULL.\n");
    }
} // END OF FUNCTION


// ===================================================================================================================================== //


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void tipo_norma_load_storage(ArrayList* alistTipoNorma) {

    if(alistTipoNorma == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT * FROM tipo_norma");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        TipoNorma* nTipoNorma = newTipoNorma();

        if (nTipoNorma != NULL) {
            nTipoNorma->id = atoi(row[0]);
            strncpy(nTipoNorma->descripcion, row[1], sizeof(nTipoNorma->descripcion) -1);

            alistTipoNorma->add(alistTipoNorma, nTipoNorma);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d TIPO NORMA cargadas. Espacio reservado: %d slots.\n", alistTipoNorma->len(alistTipoNorma), alistTipoNorma->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
static void get_tipo_norma_value(const char *body, const char *key, char *out, size_t out_size) {

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
// FUNCION PARA REGISTRAR NUEVA FUNCION EJECUTIVA
// ===================================================================================================================================== //
int tipo_norma_service_register(const char *body, char *error_msg, int error_size) {

    char descripcion[101];
    char d_descripcion[101];

    // CAPTURAMOS LOS DATOS
    get_tipo_norma_value(body, "descripcion", descripcion, sizeof(descripcion));
    url_decode(d_descripcion, descripcion);


    // EVALUAMOS SI LLEGA ALGÚN CAMPO VACIO
    if (strlen(d_descripcion) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListTipoNormaLocal != NULL) {

        for (int i = 0; i < pListTipoNormaLocal->len(pListTipoNormaLocal); i++) {

            TipoNorma* nTipoNorma = (TipoNorma*) pListTipoNormaLocal->get(pListTipoNormaLocal, i);

            if (strcasecmp(nTipoNorma->descripcion, d_descripcion) == 0) {

                snprintf(error_msg, error_size, "Error: Registro existente.");
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS

    int nuevo_id = sp_insertar_tipo_norma(d_descripcion);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    TipoNorma* nuevoTipoNorma = newTipoNorma();

    if (nuevoTipoNorma) {

        // Aprovechamos para inicializar el bloque de memoria limpio
        memset(nuevoTipoNorma, 0, sizeof(TipoNorma));

        nuevoTipoNorma->id = nuevo_id;
        strncpy(nuevoTipoNorma->descripcion, d_descripcion, sizeof(nuevoTipoNorma->descripcion) -1);

        // Sincronizamos el ArrayList inmediatamente
        pListTipoNormaLocal->add(pListTipoNormaLocal, nuevoTipoNorma);

        printf("✅ Sincronización exitosa: TIPO NORMA (ID: %d) añadida a RAM.\n", nuevo_id);
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// FUNCION EDICIÓN DE REGISTRO
// ===================================================================================================================================== //
int tipo_norma_service_edit(const char *body, char *error_msg, int error_size) {

    char id_str[32];
    char descripcion[101];
    char d_descripcion[101];

    get_tipo_norma_value(body, "id", id_str, sizeof(id_str));

    get_tipo_norma_value(body, "descripcion", descripcion, sizeof(descripcion));
    url_decode(d_descripcion, descripcion);

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_descripcion) == 0) {
        snprintf(error_msg, error_size, "ID, o Alguno de los otros campos no contienen datos");
        return 0;
    }

    // 2. VERIFICACIÓN DE EXISTENCIA Y DUPLICADOS EN MEMORIA
    if (pListTipoNormaLocal != NULL) {
        for (int i = 0; i < pListTipoNormaLocal->len(pListTipoNormaLocal); i++) {
            TipoNorma* nTipoNorma = (TipoNorma*) pListTipoNormaLocal->get(pListTipoNormaLocal, i);

            // Si el nombre ya existe en otro ID, rebotamos la edición
            if (nTipoNorma->id != id_a_editar
                    && strcasecmp(nTipoNorma->descripcion, d_descripcion) == 0) {
                snprintf(error_msg, error_size, "Error: Registro Existente.");
                return 0;
            }
        }
    }

    // 3. ACTUALIZAR EN BASE DE DATOS (Blindado)
    // CORRECCIÓN 2: Se pasa d_dedicacion_funcional_ur en lugar de dedicacion_funcional_ur
    if (sp_editar_tipo_norma(id_a_editar, d_descripcion) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 4. ACTUALIZAR EN MEMORIA (ArrayList)
    if (pListTipoNormaLocal != NULL) {
        for (int i = 0; i < pListTipoNormaLocal->len(pListTipoNormaLocal); i++) {
            TipoNorma* nTipoNorma = (TipoNorma*) pListTipoNormaLocal->get(pListTipoNormaLocal, i);

            if (nTipoNorma->id == id_a_editar) {
                // Actualizamos el puntero directamente en la memoria
                strncpy(nTipoNorma->descripcion, d_descripcion, sizeof(nTipoNorma->descripcion) -1);
                printf("✅ Memoria sincronizada: TIPO NORMA ID %d actualizado.\n", id_a_editar);
                break;
            }
        }
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_tipo_norma_service_id(const char *body, char *json_out, int out_size) {

    char id_str[10];
    get_tipo_norma_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512];

    snprintf(query, sizeof(query),
             "SELECT * FROM tipo_norma WHERE id = %s LIMIT 1;", id_str);

    DBResult *res = db_query(query);

    // 🔹 CORRECCIÓN AQUÍ: Usamos db_fetch_row o la función correspondiente de tu db.h
    // Si tu db.h usa mysql_fetch_row directamente:
    MYSQL_ROW row;

    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Registro no encontrado\" }");
        if (res) db_free_result(res);
        return 0;
    }

    // Construimos el JSON usando los índices del array 'row'
    snprintf(json_out, out_size,
             "{ \"id\": %s, \"descripcion\": \"%s\"}",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "");

    db_free_result(res);
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_tipo_norma(int client, const char *body) {

    char error_msg[256];

    if (tipo_norma_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Tipo Norma creado y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// Handler POST para la edición
// ===================================================================================================================================== //
static void route_post_tipo_norma_edit(int client, const char *body) {

    char error_msg[256];

    if (tipo_norma_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Tipo Norma actualizado correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES FOR LIST
// ===================================================================================================================================== //
static void route_get_tipo_norma_list(int client, const char *body) {


    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListTipoNormaLocal == NULL) {
        printf("❌ Error crítico: pListTipoNormaLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    // Estimamos el tamaño del JSON (aprox 150 bytes por médico)
    size_t total_registros = pListTipoNormaLocal->len(pListTipoNormaLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {

        TipoNorma* oneTipoNorma = (TipoNorma*) pListTipoNormaLocal->get(pListTipoNormaLocal, i);

        char item[600];

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"descripcion\": \"%s\"  }%s",
            oneTipoNorma->id, oneTipoNorma->descripcion,  (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON


} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTE OR GET ONE REGESTRY
// ===================================================================================================================================== //
static void route_get_tipo_norma_by_id(int client, const char *body) {

    char response_json[1024];

    if (get_tipo_norma_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }

} //END OF FUNCTION


// ===================================================================================================================================== //
// INIT ALL ROUTES
// ===================================================================================================================================== //
void init_tipo_norma_routes() {

    add_route("GET", "/tipo_norma/list", route_get_tipo_norma_list); // endpoint para listar
    add_route("POST", "/tipo_norma/add", route_post_tipo_norma); // endpoint para alta de nuevo registro
    add_route("POST", "/tipo_norma/edit", route_post_tipo_norma_edit); // endpoint para editar un registro
    add_route("POST", "/tipo_norma/get", route_get_tipo_norma_by_id); // endpoint para consultar un registro por ID
}
