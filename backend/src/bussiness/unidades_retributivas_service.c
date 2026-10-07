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
#include "unidades_retributivas_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListUnidadesRetributivasLocal = NULL;

// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (UNIDADES RETRIBUTIVAS)
// ===================================================================================================================================== //
static int sp_insertar_unidades_retributivas(const char* nivel, const char* grado, const char* sueldo_ur, const char* dedicacion_funcional_ur, const char* total_ur) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[5];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_unidades_retributivas(?, ?, ?, ?, ?)";

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
    bind_param[0].buffer = (char *)nivel;
    bind_param[0].buffer_length = strlen(nivel);

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)grado;
    bind_param[1].buffer_length = strlen(grado);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)sueldo_ur;
    bind_param[2].buffer_length = strlen(sueldo_ur);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)dedicacion_funcional_ur;
    bind_param[3].buffer_length = strlen(dedicacion_funcional_ur);

    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = (char *)total_ur;
    bind_param[4].buffer_length = strlen(total_ur);


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


static int sp_editar_unidades_retributivas(int id, const char* nivel, const char* grado, const char* sueldo_ur, const char* dedicacion_funcional_ur, const char* total_ur) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[6];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_editar_unidades_retributivas(?, ?, ?, ?, ?, ?)";

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
    bind_param[0].buffer = (void *)&id;
    bind_param[0].is_unsigned = 0;

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)nivel;
    bind_param[1].buffer_length = strlen(nivel);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)grado;
    bind_param[2].buffer_length = strlen(grado);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)sueldo_ur;
    bind_param[3].buffer_length = strlen(sueldo_ur);

    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = (char *)dedicacion_funcional_ur;
    bind_param[4].buffer_length = strlen(dedicacion_funcional_ur);

    bind_param[5].buffer_type = MYSQL_TYPE_STRING;
    bind_param[5].buffer = (char *)total_ur;
    bind_param[5].buffer_length = strlen(total_ur);


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


// ===================================================================================================================================== //
// CONSTRUCTOR
// ===================================================================================================================================== //
UnidadesRetributivas* newUnidadRetributiva(){

    UnidadesRetributivas* nUnidadRetributiva = (UnidadesRetributivas*)malloc(sizeof(UnidadesRetributivas));

    if(nUnidadRetributiva != NULL){
        memset(nUnidadRetributiva, 0, sizeof(UnidadesRetributivas));
    }

    return nUnidadRetributiva;

} // END OF FUNCTION


// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void unidades_retributivas_init_cache(ArrayList* alistUnidadesRetributivas) {

    if(alistUnidadesRetributivas != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListUnidadesRetributivasLocal = alistUnidadesRetributivas;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ UNIDADES RETRIBUTIVAS ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ UNIDADES RETRIBUTIVAS ] con NULL.\n");
    }
} // END OF FUNCTION


// ===================================================================================================================================== //


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void unidades_retributivas_load_storage(ArrayList* alistUnidadesRetributivas) {

    if(alistUnidadesRetributivas == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT * FROM unidades_retributivas");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        UnidadesRetributivas* nUnidadRetributiva = newUnidadRetributiva();

        if (nUnidadRetributiva != NULL) {
            nUnidadRetributiva->id = atoi(row[0]);
            strncpy(nUnidadRetributiva->nivel, row[1], sizeof(nUnidadRetributiva->nivel) -1);
            strncpy(nUnidadRetributiva->grado, row[2], sizeof(nUnidadRetributiva->grado) -1);
            nUnidadRetributiva->sueldo_ur = atoi(row[3]);
            nUnidadRetributiva->dedicacion_funcional_ur = atoi(row[4]);
            nUnidadRetributiva->total_ur = atoi(row[5]);

            alistUnidadesRetributivas->add(alistUnidadesRetributivas, nUnidadRetributiva);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d UNIDADES RETRIBUTIVAS cargadas. Espacio reservado: %d slots.\n", alistUnidadesRetributivas->len(alistUnidadesRetributivas), alistUnidadesRetributivas->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
static void get_unidades_retributivas_value(const char *body, const char *key, char *out, size_t out_size) {

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
int unidades_retributivas_service_register(const char *body, char *error_msg, int error_size) {

    char nivel[2];
    char d_nivel[2];
    char grado[3];
    char d_grado[3];
    char sueldo_ur[11];
    char d_sueldo_ur[11];
    char dedicacion_funcional_ur[11];
    char d_dedicacion_funcional_ur[11];
    char total_ur[11];
    char d_total_ur[11];


    get_unidades_retributivas_value(body,"nivel", nivel, sizeof(nivel));
    url_decode(d_nivel, nivel);

    get_unidades_retributivas_value(body,"grado", grado, sizeof(grado));
    url_decode(d_grado, grado);

    get_unidades_retributivas_value(body,"sueldo_ur", sueldo_ur, sizeof(sueldo_ur));
    url_decode(d_sueldo_ur, sueldo_ur);

    get_unidades_retributivas_value(body,"dedicacion_funcional_ur", dedicacion_funcional_ur, sizeof(dedicacion_funcional_ur));
    url_decode(d_dedicacion_funcional_ur, dedicacion_funcional_ur);

    get_unidades_retributivas_value(body, "total_ur", total_ur, sizeof(total_ur));
    url_decode(d_total_ur, total_ur); // Asumiendo tu función de decode



    if (strlen(d_nivel) == 0 || strlen(d_grado) == 0 || strlen(d_sueldo_ur) == 0 || strlen(d_dedicacion_funcional_ur) == 0 || strlen(d_total_ur) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListUnidadesRetributivasLocal != NULL) {

        for (int i = 0; i < pListUnidadesRetributivasLocal->len(pListUnidadesRetributivasLocal); i++) {

            UnidadesRetributivas* nUnidades = (UnidadesRetributivas*) pListUnidadesRetributivasLocal->get(pListUnidadesRetributivasLocal, i);

            if (strcasecmp(nUnidades->nivel, d_nivel) == 0 && strcasecmp(nUnidades->grado, d_grado) == 0 && nUnidades->sueldo_ur == atoi(d_sueldo_ur) && nUnidades->dedicacion_funcional_ur == atoi(d_dedicacion_funcional_ur) && nUnidades->total_ur == atoi(d_total_ur)) {

                snprintf(error_msg, error_size, "Error: Registro existente.");
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS

    int nuevo_id = sp_insertar_unidades_retributivas(d_nivel, d_grado, d_sueldo_ur, d_dedicacion_funcional_ur, d_total_ur);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    UnidadesRetributivas* nuevaUnidad = newUnidadRetributiva();

    if (nuevaUnidad) {

        // Aprovechamos para inicializar el bloque de memoria limpio
        memset(nuevaUnidad, 0, sizeof(UnidadesRetributivas));

        nuevaUnidad->id = nuevo_id;
        strncpy(nuevaUnidad->nivel, d_nivel, sizeof(nuevaUnidad->nivel) -1);
        strncpy(nuevaUnidad->grado, d_grado, sizeof(nuevaUnidad->grado) -1);
        nuevaUnidad->sueldo_ur = atoi(d_sueldo_ur);
        nuevaUnidad->dedicacion_funcional_ur = atoi(d_dedicacion_funcional_ur);
        nuevaUnidad->total_ur = atoi(d_total_ur);

        // Sincronizamos el ArrayList inmediatamente
        pListUnidadesRetributivasLocal->add(pListUnidadesRetributivasLocal, nuevaUnidad);

        printf("✅ Sincronización exitosa: UNIDAD RETRIBUTIVA (ID: %d) añadida a RAM.\n", nuevo_id);
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// FUNCION EDICIÓN DE REGISTRO
// ===================================================================================================================================== //
int unidades_retributivas_service_edit(const char *body, char *error_msg, int error_size) {

    char id_str[32];
    char nivel[2];
    char d_nivel[2];
    char grado[3];
    char d_grado[3];
    char sueldo_ur[11];
    char d_sueldo_ur[11];
    char dedicacion_funcional_ur[11];
    char d_dedicacion_funcional_ur[11];
    char total_ur[11];
    char d_total_ur[11];

    get_unidades_retributivas_value(body,"id", id_str, sizeof(id_str));

    get_unidades_retributivas_value(body,"nivel", nivel, sizeof(nivel));
    url_decode(d_nivel, nivel);

    get_unidades_retributivas_value(body,"grado", grado, sizeof(grado));
    url_decode(d_grado, grado);

    get_unidades_retributivas_value(body, "sueldo_ur", sueldo_ur, sizeof(sueldo_ur));
    url_decode(d_sueldo_ur, sueldo_ur);

    get_unidades_retributivas_value(body, "dedicacion_funcional_ur", dedicacion_funcional_ur, sizeof(d_dedicacion_funcional_ur));
    url_decode(d_dedicacion_funcional_ur, dedicacion_funcional_ur);

    get_unidades_retributivas_value(body, "total_ur", total_ur, sizeof(total_ur));
    url_decode(d_total_ur, total_ur);

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_nivel) == 0 || strlen(d_grado) == 0 || strlen(d_sueldo_ur) == 0 || strlen(d_dedicacion_funcional_ur) == 0 || strlen(d_total_ur) == 0) {
        snprintf(error_msg, error_size, "ID, o Alguno de los otros campos no contienen datos");
        return 0;
    }

    // 2. VERIFICACIÓN DE EXISTENCIA Y DUPLICADOS EN MEMORIA
    if (pListUnidadesRetributivasLocal != NULL) {
        for (int i = 0; i < pListUnidadesRetributivasLocal->len(pListUnidadesRetributivasLocal); i++) {
            UnidadesRetributivas* nUnidades = (UnidadesRetributivas*) pListUnidadesRetributivasLocal->get(pListUnidadesRetributivasLocal, i);

            // Si el nombre ya existe en otro ID, rebotamos la edición
            if (nUnidades->id != id_a_editar && strcasecmp(nUnidades->nivel, d_nivel) == 0 && strcasecmp(nUnidades->grado, d_grado) == 0 && nUnidades->sueldo_ur == atoi(d_sueldo_ur) && nUnidades->dedicacion_funcional_ur == atoi(d_dedicacion_funcional_ur) && nUnidades->total_ur == atoi(d_total_ur)) {
                snprintf(error_msg, error_size, "Error: Registro Existente.");
                return 0;
            }
        }
    }

    // 3. ACTUALIZAR EN BASE DE DATOS (Blindado)
    // CORRECCIÓN 2: Se pasa d_dedicacion_funcional_ur en lugar de dedicacion_funcional_ur
    if (sp_editar_unidades_retributivas(id_a_editar, d_nivel, d_grado, d_sueldo_ur, d_dedicacion_funcional_ur, d_total_ur) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 4. ACTUALIZAR EN MEMORIA (ArrayList)
    if (pListUnidadesRetributivasLocal != NULL) {
        for (int i = 0; i < pListUnidadesRetributivasLocal->len(pListUnidadesRetributivasLocal); i++) {
            UnidadesRetributivas* nUnidades = (UnidadesRetributivas*) pListUnidadesRetributivasLocal->get(pListUnidadesRetributivasLocal, i);

            if (nUnidades->id == id_a_editar) {
                // Actualizamos el puntero directamente en la memoria
                strncpy(nUnidades->nivel, d_nivel, sizeof(nUnidades->nivel) -1);
                strncpy(nUnidades->grado, d_grado, sizeof(nUnidades->grado) -1);
                nUnidades->sueldo_ur = atoi(d_sueldo_ur);
                nUnidades->dedicacion_funcional_ur = atoi(d_dedicacion_funcional_ur);
                nUnidades->total_ur = atoi(d_total_ur);
                printf("✅ Memoria sincronizada: UNIDAD RETRIBUTIVA ID %d actualizado.\n", id_a_editar);
                break;
            }
        }
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_unidades_retributivas_service_id(const char *body, char *json_out, int out_size) {

    char id_str[10];
    get_unidades_retributivas_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512];

    snprintf(query, sizeof(query),
             "SELECT * FROM unidades_retributivas WHERE id = %s LIMIT 1;", id_str);

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
             "{ \"id\": %s, \"nivel\": \"%s\", \"grado\": \"%s\",  \"sueldo_ur\": \"%s\",  \"dedicacion_funcional_ur\": \"%s\", \"total_ur\": \"%s\"}",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "",
             row[2] ? row[2] : "",
             row[3] ? row[3] : "0",
             row[4] ? row[4] : "0",
             row[5] ? row[5] : "0");

    db_free_result(res);
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_unidades_retributivas(int client, const char *body) {

    char error_msg[256];

    if (unidades_retributivas_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Unidad Retributiva creada y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// Handler POST para la edición
// ===================================================================================================================================== //
static void route_post_unidades_retributivas_edit(int client, const char *body) {

    char error_msg[256];

    if (unidades_retributivas_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Unidad Retributiva actualizada correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES FOR LIST
// ===================================================================================================================================== //
static void route_get_unidades_retributivas_list(int client, const char *body) {


    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListUnidadesRetributivasLocal == NULL) {
        printf("❌ Error crítico: pListUnidadesRetributivasLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    // Estimamos el tamaño del JSON (aprox 150 bytes por médico)
    size_t total_registros = pListUnidadesRetributivasLocal->len(pListUnidadesRetributivasLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {

        UnidadesRetributivas* oneUnidad = (UnidadesRetributivas*) pListUnidadesRetributivasLocal->get(pListUnidadesRetributivasLocal, i);

        char item[600];

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"nivel\": \"%s\", \"grado\": \"%s\" , \"sueldo_ur\": \"%d\", \"dedicacion_funcional_ur\": \"%d\", \"total_ur\": \"%d\"  }%s",
            oneUnidad->id, oneUnidad->nivel, oneUnidad->grado, oneUnidad->sueldo_ur, oneUnidad->dedicacion_funcional_ur, oneUnidad->total_ur,  (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON


} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTE OR GET ONE REGESTRY
// ===================================================================================================================================== //
static void route_get_unidades_retributivas_by_id(int client, const char *body) {

    char response_json[1024];

    if (get_unidades_retributivas_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }

} //END OF FUNCTION


// ===================================================================================================================================== //
// INIT ALL ROUTES
// ===================================================================================================================================== //
void init_unidades_retributivas_routes() {

    add_route("GET", "/unidades_retributivas/list", route_get_unidades_retributivas_list); // endpoint para listar
    add_route("POST", "/unidades_retributivas/add", route_post_unidades_retributivas); // endpoint para alta de nuevo registro
    add_route("POST", "/unidades_retributivas/edit", route_post_unidades_retributivas_edit); // endpoint para editar un registro
    add_route("POST", "/unidades_retributivas/get", route_get_unidades_retributivas_by_id); // endpoint para consultar un registro por ID
}
