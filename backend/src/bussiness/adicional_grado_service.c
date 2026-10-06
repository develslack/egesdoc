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
#include "adicional_grado_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListAdicionalGradoLocal = NULL;

// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (ADICIONAL GRADO)
// ===================================================================================================================================== //
static int sp_insertar_adicional_grado(const char* nivel, const char* grado, const char* cant_ur) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[3];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_funcion_ejecutiva(?, ?, ?)";

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

    memset(bind_param, 0, sizeof(bind_param));
    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)grado;
    bind_param[1].buffer_length = strlen(grado);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)cant_ur;
    bind_param[2].buffer_length = strlen(cant_ur);


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


static int sp_editar_adicional_grado(int id, const char* nivel, const char* grado, const char* cant_ur) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[4];
    const char *query = "CALL sp_editar_adicional_grado(?, ?, ?, ?)";

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
    bind_param[1].buffer = (char *)nivel;
    bind_param[1].buffer_length = strlen(nivel);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)grado;
    bind_param[2].buffer_length = strlen(grado);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)cant_ur;
    bind_param[3].buffer_length = strlen(cant_ur);


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


// CONSTRUCTOR
// ===================================================================================================================================== //
AdicionalGrado* newAdicionalGrado(){

    AdicionalGrado* oneAdicionalGrado = (AdicionalGrado*)malloc(sizeof(AdicionalGrado));

    if(oneAdicionalGrado != NULL){

        memset(oneAdicionalGrado, 0, sizeof(AdicionalGrado));
    }

    return oneAdicionalGrado;

} // END OF FUNCTION


// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void adicional_grado_init_cache(ArrayList* alistAdicionalGrado) {

    if(alistAdicionalGrado != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListAdicionalGradoLocal = alistAdicionalGrado;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ Adicional Grado ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ Adicional Grado ] con NULL.\n");
    }
} // END OF FUNCTION


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void adicional_grado_load_storage(ArrayList* alistAdicionalGrado) {

    if(alistAdicionalGrado == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT * FROM adicional_grado_ur");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        AdicionalGrado* nAdicionalGrado = newAdicionalGrado();

        if (nAdicionalGrado != NULL) {
            nAdicionalGrado->id = atoi(row[0]);
            strncpy(nAdicionalGrado->nivel, row[1], sizeof(nAdicionalGrado->nivel) -1);
            strncpy(nAdicionalGrado->grado, row[2], sizeof(nAdicionalGrado->grado) -1);
            nAdicionalGrado->cant_ur = atoi(row[3]);

            alistAdicionalGrado->add(alistAdicionalGrado, nAdicionalGrado);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d ADICIONAL GRADO cargados. Espacio reservado: %d slots.\n", alistAdicionalGrado->len(alistAdicionalGrado), alistAdicionalGrado->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
static void get_adicional_grado_value(const char *body, const char *key, char *out, size_t out_size) {

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
int adicional_grado_service_register(const char *body, char *error_msg, int error_size) {

    char nivel[2];
    char d_nivel[2];
    char grado[3];
    char d_grado[3];
    char cant_ur[11];
    char d_cant_ur[11];


    get_adicional_grado_value(body,"nivel", nivel, sizeof(nivel));
    url_decode(d_nivel, nivel);

    get_adicional_grado_value(body,"grado", nivel, sizeof(grado));
    url_decode(d_nivel, nivel);

    get_adicional_grado_value(body, "cant_ur", cant_ur, sizeof(cant_ur));
    url_decode(d_grado, grado); // Asumiendo tu función de decode



    if (strlen(d_nivel) == 0 || strlen(d_grado) == 0 || strlen(d_cant_ur) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListAdicionalGradoLocal != NULL) {

        for (int i = 0; i < pListAdicionalGradoLocal->len(pListAdicionalGradoLocal); i++) {

            AdicionalGrado* nAdicional = (AdicionalGrado*) pListAdicionalGradoLocal->get(pListAdicionalGradoLocal, i);

            if (strcasecmp(nAdicional->nivel, d_nivel) == 0 && strcasecmp(nAdicional->grado, d_grado) == 0 && nAdicional->cant_ur == atoi(d_cant_ur)) {

                snprintf(error_msg, error_size, "Error: Función Ejecutiva existente.");
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS

    int nuevo_id = sp_insertar_adicional_grado(d_nivel, d_grado, d_cant_ur);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    AdicionalGrado* nuevoAdicional = newAdicionalGrado();

    if (nuevoAdicional) {

        // Aprovechamos para inicializar el bloque de memoria limpio
        memset(nuevoAdicional, 0, sizeof(AdicionalGrado));

        nuevoAdicional->id = nuevo_id;
        strncpy(nuevoAdicional->nivel, d_nivel, sizeof(nuevoAdicional->nivel) -1);
        strncpy(nuevoAdicional->grado, d_grado, sizeof(nuevoAdicional->grado) -1);
        nuevoAdicional->cant_ur = atoi(d_cant_ur);

        // Sincronizamos el ArrayList inmediatamente
        pListAdicionalGradoLocal->add(pListAdicionalGradoLocal, nuevoAdicional);

        printf("✅ Sincronización exitosa: ADICIONAL GRADO (ID: %d) añadida a RAM.\n", nuevo_id);
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// FUNCION EDICIÓN DE ACTIVIDAD
// ===================================================================================================================================== //
int adicional_grado_service_edit(const char *body, char *error_msg, int error_size) {

    char id_str[32];
    char nivel[2];
    char d_nivel[2];
    char grado[3];
    char d_grado[3];
    char cant_ur[11];
    char d_cant_ur[11];


    get_adicional_grado_value(body,"id", nivel, sizeof(id_str));

    get_adicional_grado_value(body,"nivel", nivel, sizeof(nivel));
    url_decode(d_nivel, nivel);

    get_adicional_grado_value(body,"grado", grado, sizeof(grado));
    url_decode(d_grado, grado);

    get_adicional_grado_value(body, "cant_ur", cant_ur, sizeof(cant_ur));
    url_decode(d_cant_ur, cant_ur); // Asumiendo tu función de decode

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_nivel) == 0 || strlen(d_grado) == 0 || strlen(d_cant_ur) == 0) {
        snprintf(error_msg, error_size, "ID, o Alguno de los otros campos no contienen datos");
        return 0;
    }

    // 2. VERIFICACIÓN DE EXISTENCIA Y DUPLICADOS EN MEMORIA
    if (pListAdicionalGradoLocal != NULL) {

        for (int i = 0; i < pListAdicionalGradoLocal->len(pListAdicionalGradoLocal); i++) {

            AdicionalGrado* nAdicional = (AdicionalGrado*) pListAdicionalGradoLocal->get(pListAdicionalGradoLocal, i);

            // Si el nombre ya existe en otro ID, rebotamos la edición
            if (nAdicional->id != id_a_editar && strcasecmp(nAdicional->nivel, d_nivel) == 0 && strcasecmp(nAdicional->nivel, d_nivel) == 0 && nAdicional->cant_ur == atoi(d_cant_ur)) {

                snprintf(error_msg, error_size, "Error: Registro Existente.");
                return 0;
            }
        }
    }

    // 3. ACTUALIZAR EN BASE DE DATOS (Blindado)
    if (sp_editar_adicional_grado(id_a_editar, d_nivel, d_grado, d_cant_ur) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 4. ACTUALIZAR EN MEMORIA (ArrayList)
    if (pListAdicionalGradoLocal != NULL) {

        for (int i = 0; i < pListAdicionalGradoLocal->len(pListAdicionalGradoLocal); i++) {

            AdicionalGrado* nAdicional = (AdicionalGrado*) pListAdicionalGradoLocal->get(pListAdicionalGradoLocal, i);

            if (nAdicional->id == id_a_editar) {
                // Actualizamos el puntero directamente en la memoria
                strncpy(nAdicional->nivel, d_nivel, sizeof(nAdicional->nivel) -1);
                strncpy(nAdicional->grado, d_grado, sizeof(nAdicional->grado) -1);
                nAdicional->cant_ur = atoi(d_cant_ur);
                printf("✅ Memoria sincronizada: Adicional Grado ID %d actualizado.\n", id_a_editar);
                break;
            }
        }
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_adicional_grado_service_id(const char *body, char *json_out, int out_size) {

    char id_str[10];
    get_adicional_grado_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512];

    snprintf(query, sizeof(query),
             "SELECT * FROM adicional_grado_ur WHERE id = %s LIMIT 1;", id_str);

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
             "{ \"id\": %s, \"nivel\": \"%s\", \"grado\": \"%s\", \"cant_ur\": \"%s\"}",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "",
             row[2] ? row[2] : "",
             row[3] ? row[3] : "0");

    db_free_result(res);
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_adicional_grado(int client, const char *body) {

    char error_msg[256];

    if (adicional_grado_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Adicional Grado creado y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// Handler POST para la edición
// ===================================================================================================================================== //
static void route_post_adicional_grado_edit(int client, const char *body) {

    char error_msg[256];

    if (adicional_grado_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Adicional Grado actualizado correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES FOR LIST
// ===================================================================================================================================== //
static void route_get_adicional_grado_list(int client, const char *body) {


    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListAdicionalGradoLocal == NULL) {
        printf("❌ Error crítico: pListAdicionalGradoLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    // Estimamos el tamaño del JSON (aprox 150 bytes por médico)
    size_t total_registros = pListAdicionalGradoLocal->len(pListAdicionalGradoLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {

        AdicionalGrado* oneAdicionalGrado = (AdicionalGrado*) pListAdicionalGradoLocal->get(pListAdicionalGradoLocal, i);

        char item[600];

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"nivel\": \"%s\", \"grado\": \"%s\" , \"cant_ur\": \"%d\"  }%s",
            oneAdicionalGrado->id, oneAdicionalGrado->nivel, oneAdicionalGrado->grado, oneAdicionalGrado->cant_ur,  (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON


} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTE OR GET ONE REGESTRY
// ===================================================================================================================================== //
static void route_get_adicional_grado_by_id(int client, const char *body) {

    char response_json[1024];

    if (get_adicional_grado_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }

} //END OF FUNCTION


// ===================================================================================================================================== //
// INIT ALL ROUTES
// ===================================================================================================================================== //
void init_adicional_grado_routes() {

    add_route("GET", "/adicional_grado/list", route_get_adicional_grado_list); // endpoint para listar
    add_route("POST", "/adicional_grado/add", route_post_adicional_grado); // endpoint para alta de nuevo registro
    add_route("POST", "/adicional_grado/edit", route_post_adicional_grado_edit); // endpoint para editar un registro
    add_route("POST", "/adicional_grado/get", route_get_adicional_grado_by_id); // endpoint para consultar un registro por ID
}
