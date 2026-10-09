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
#include "organismos_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListOrganismosLocal = NULL;


// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (ORGANISMO)
// ===================================================================================================================================== //
static int sp_insertar_organismo(const char* cod_org, const char* saf, const char* descripcion, const char* ubicacion_fisica) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[4];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_organismo(?,?,?,?)";

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
    bind_param[0].buffer = (char *)cod_org;
    bind_param[0].buffer_length = strlen(cod_org);

    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)saf;
    bind_param[1].buffer_length = strlen(saf);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)descripcion;
    bind_param[2].buffer_length = strlen(descripcion);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)ubicacion_fisica;
    bind_param[3].buffer_length = strlen(ubicacion_fisica);

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


static int sp_editar_organismo(int id, const char* cod_org, const char* saf, const char* descripcion, const char* ubicacion_fisica) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[5];
    const char *query = "CALL sp_editar_organismo(?,?,?,?,?)";

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
    bind_param[1].buffer = (char *)cod_org;
    bind_param[1].buffer_length = strlen(cod_org);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)saf;
    bind_param[2].buffer_length = strlen(saf);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)descripcion;
    bind_param[3].buffer_length = strlen(descripcion);

    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = (char *)ubicacion_fisica;
    bind_param[4].buffer_length = strlen(ubicacion_fisica);


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
Organismos* newOrganismo(){

    Organismos* nOrganismo = (Organismos*)malloc(sizeof(Organismos));

    if(nOrganismo != NULL){
        memset(nOrganismo, 0, sizeof(Organismos));
    }

    return nOrganismo;

} // END OF FUNCTION


// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void organismos_init_cache(ArrayList* alistOrganismos) {

    if(alistOrganismos != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListOrganismosLocal = alistOrganismos;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ ORGANISMOS ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ ORGANISMOS ] con NULL.\n");
    }
} // END OF FUNCTION


// ===================================================================================================================================== //


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void organismos_load_storage(ArrayList* alistOrganismos) {

    if(alistOrganismos == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT * FROM organismos");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        Organismos* nOrganismo = newOrganismo();

        if (nOrganismo != NULL) {
            nOrganismo->id = atoi(row[0]);
            strncpy(nOrganismo->cod_org, row[1], sizeof(nOrganismo->cod_org) -1);
            strncpy(nOrganismo->saf, row[2], sizeof(nOrganismo->saf) -1);
            strncpy(nOrganismo->descripcion, row[3], sizeof(nOrganismo->descripcion) -1);
            strncpy(nOrganismo->ubicacion_fisica, row[4], sizeof(nOrganismo->ubicacion_fisica) -1);

            alistOrganismos->add(alistOrganismos, nOrganismo);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d ORGANISMOS cargados. Espacio reservado: %d slots.\n", alistOrganismos->len(alistOrganismos), alistOrganismos->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
static void get_organismo_value(const char *body, const char *key, char *out, size_t out_size) {

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
// FUNCION PARA INSERTAR NUEVO REGISTRO
// ===================================================================================================================================== //
int organismos_service_register(const char *body, char *error_msg, int error_size) {

    char cod_org[3];
    char d_cod_org[3];
    char saf[5];
    char d_saf[5];
    char descripcion[301];
    char d_descripcion[301];
    char ubicacion_fisica[121];
    char d_ubicacion_fisica[121];

    // CAPTURAMOS LOS DATOS
    get_organismo_value(body, "cod_org", cod_org, sizeof(cod_org));
    url_decode(d_cod_org, cod_org);

    get_organismo_value(body, "saf", saf, sizeof(saf));
    url_decode(d_saf, saf);

    get_organismo_value(body, "descripcion", descripcion, sizeof(descripcion));
    url_decode(d_descripcion, descripcion);

    get_organismo_value(body, "ubicacion_fisica", ubicacion_fisica, sizeof(ubicacion_fisica));
    url_decode(d_ubicacion_fisica, ubicacion_fisica);


    // EVALUAMOS SI LLEGA ALGÚN CAMPO VACIO
    if (strlen(d_cod_org) == 0 || strlen(d_saf) == 0 || strlen(d_descripcion) == 0 || strlen(d_ubicacion_fisica) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListOrganismosLocal != NULL) {

        for (int i = 0; i < pListOrganismosLocal->len(pListOrganismosLocal); i++) {

            Organismos* nOrganismo = (Organismos*) pListOrganismosLocal->get(pListOrganismosLocal, i);

            if (strcasecmp(nOrganismo->cod_org, d_cod_org) == 0 && strcasecmp(nOrganismo->saf, d_saf) == 0 && strcasecmp(nOrganismo->descripcion, d_descripcion) == 0 && strcasecmp(nOrganismo->ubicacion_fisica, d_ubicacion_fisica) == 0) {

                snprintf(error_msg, error_size, "Error: Registro existente.");
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS

    int nuevo_id = sp_insertar_organismo(d_cod_org, d_saf, d_descripcion, d_ubicacion_fisica);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    Organismos* nuevoOrganismo = newOrganismo();

    if (nuevoOrganismo) {

        // Aprovechamos para inicializar el bloque de memoria limpio
        memset(nuevoOrganismo, 0, sizeof(Organismos));

        nuevoOrganismo->id = nuevo_id;
        strncpy(nuevoOrganismo->cod_org, d_cod_org, sizeof(nuevoOrganismo->cod_org) -1);
        strncpy(nuevoOrganismo->saf, d_saf, sizeof(nuevoOrganismo->saf) -1);
        strncpy(nuevoOrganismo->descripcion, d_descripcion, sizeof(nuevoOrganismo->descripcion) -1);
        strncpy(nuevoOrganismo->ubicacion_fisica, d_ubicacion_fisica, sizeof(nuevoOrganismo->ubicacion_fisica) -1);

        // Sincronizamos el ArrayList inmediatamente
        pListOrganismosLocal->add(pListOrganismosLocal, nuevoOrganismo);

        printf("✅ Sincronización exitosa: ORGANISMO (ID: %d) añadidO a RAM.\n", nuevo_id);
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// FUNCION EDICIÓN DE REGISTRO
// ===================================================================================================================================== //
int organismos_service_edit(const char *body, char *error_msg, int error_size) {

    char id_str[32];
    char cod_org[3];
    char d_cod_org[3];
    char saf[5];
    char d_saf[5];
    char descripcion[301];
    char d_descripcion[301];
    char ubicacion_fisica[121];
    char d_ubicacion_fisica[121];

    get_organismo_value(body, "id", id_str, sizeof(id_str));

    get_organismo_value(body, "cod_org", cod_org, sizeof(cod_org));
    url_decode(d_cod_org, cod_org);

    get_organismo_value(body, "saf", saf, sizeof(saf));
    url_decode(d_saf, saf);

    get_organismo_value(body, "descripcion", descripcion, sizeof(descripcion));
    url_decode(d_descripcion, descripcion);

    get_organismo_value(body, "ubicacion_fisica", ubicacion_fisica, sizeof(ubicacion_fisica));
    url_decode(d_ubicacion_fisica, ubicacion_fisica);

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_cod_org) == 0 || strlen(d_saf) == 0 || strlen(d_descripcion) == 0 || strlen(d_ubicacion_fisica) == 0) {
        snprintf(error_msg, error_size, "ID, o Alguno de los otros campos no contienen datos");
        return 0;
    }

    // 2. VERIFICACIÓN DE EXISTENCIA Y DUPLICADOS EN MEMORIA
    if (pListOrganismosLocal != NULL) {
        for (int i = 0; i < pListOrganismosLocal->len(pListOrganismosLocal); i++) {
            Organismos* nOrganismo = (Organismos*) pListOrganismosLocal->get(pListOrganismosLocal, i);

            // Si el nombre ya existe en otro ID, rebotamos la edición
            if (nOrganismo->id != id_a_editar
                    && strcasecmp(nOrganismo->cod_org, d_cod_org) == 0
                    && strcasecmp(nOrganismo->saf, d_saf) == 0
                    && strcasecmp(nOrganismo->descripcion, d_descripcion) == 0
                    && strcasecmp(nOrganismo->ubicacion_fisica, d_ubicacion_fisica) == 0) {
                snprintf(error_msg, error_size, "Error: Registro Existente.");
                return 0;
            }
        }
    }

    // 3. ACTUALIZAR EN BASE DE DATOS (Blindado)
    // CORRECCIÓN 2: Se pasa d_dedicacion_funcional_ur en lugar de dedicacion_funcional_ur
    if (sp_editar_organismo(id_a_editar, d_cod_org, d_saf, d_descripcion, d_ubicacion_fisica) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 4. ACTUALIZAR EN MEMORIA (ArrayList)
    if (pListOrganismosLocal != NULL) {
        for (int i = 0; i < pListOrganismosLocal->len(pListOrganismosLocal); i++) {
            Organismos* nOrganismo = (Organismos*) pListOrganismosLocal->get(pListOrganismosLocal, i);

            if (nOrganismo->id == id_a_editar) {
                // Actualizamos el puntero directamente en la memoria
                strncpy(nOrganismo->cod_org, d_cod_org, sizeof(nOrganismo->cod_org) -1);
                strncpy(nOrganismo->saf, d_saf, sizeof(nOrganismo->saf) -1);
                strncpy(nOrganismo->descripcion, d_descripcion, sizeof(nOrganismo->descripcion) -1);
                strncpy(nOrganismo->ubicacion_fisica, d_ubicacion_fisica, sizeof(nOrganismo->ubicacion_fisica) -1);
                printf("✅ Memoria sincronizada: ORGANISMO ID %d actualizado.\n", id_a_editar);
                break;
            }
        }
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_organismo_service_id(const char *body, char *json_out, int out_size) {

    char id_str[10];
    get_organismo_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512];

    snprintf(query, sizeof(query),
             "SELECT * FROM organismos WHERE id = %s LIMIT 1;", id_str);

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
             "{ \"id\": %s, \"cod_org\": \"%s\", \"saf\": \"%s\", \"descripcion\": \"%s\", \"ubicacion_fisica\": \"%s\"}",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "",
             row[2] ? row[2] : "",
             row[3] ? row[3] : "",
             row[4] ? row[4] : "");

    db_free_result(res);
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_organismo(int client, const char *body) {

    char error_msg[256];

    if (organismos_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Organismo creado y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// Handler POST para la edición
// ===================================================================================================================================== //
static void route_post_organismo_edit(int client, const char *body) {

    char error_msg[256];

    if (organismos_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Organismo actualizado correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES FOR LIST
// ===================================================================================================================================== //
static void route_get_organismos_list(int client, const char *body) {


    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListOrganismosLocal == NULL) {
        printf("❌ Error crítico: pListOrganismosLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    // Estimamos el tamaño del JSON (aprox 150 bytes por médico)
    size_t total_registros = pListOrganismosLocal->len(pListOrganismosLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {

        Organismos* oneOrganismo = (Organismos*) pListOrganismosLocal->get(pListOrganismosLocal, i);

        char item[600];

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"cod_org\": \"%s\", \"saf\": \"%s\", \"descripcion\": \"%s\", \"ubicacion_fisica\": \"%s\"  }%s",
            oneOrganismo->id, oneOrganismo->cod_org, oneOrganismo->saf, oneOrganismo->descripcion, oneOrganismo->ubicacion_fisica,  (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON


} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTE OR GET ONE REGESTRY
// ===================================================================================================================================== //
static void route_get_organismo_by_id(int client, const char *body) {

    char response_json[1024];

    if (get_organismo_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }

} //END OF FUNCTION


// ===================================================================================================================================== //
// INIT ALL ROUTES
// ===================================================================================================================================== //
void init_organismos_routes() {

    add_route("GET", "/organismos/list", route_get_organismos_list); // endpoint para listar
    add_route("POST", "/organismos/add", route_post_organismo); // endpoint para alta de nuevo registro
    add_route("POST", "/organismos/edit", route_post_organismo_edit); // endpoint para editar un registro
    add_route("POST", "/organismos/get", route_get_organismo_by_id); // endpoint para consultar un registro por ID
}
