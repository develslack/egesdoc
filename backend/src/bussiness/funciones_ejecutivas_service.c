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
#include "funciones_ejecutivas_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListFuncionesEjecutivasLocal = NULL;


// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (FUNCIONES EJECUTIVAS)
// ===================================================================================================================================== //
static int sp_insertar_funcion_ejecutiva(const char* nivel, const char* cant_ur, const char* valor_ur, const char* monto, const char* norma_regulatoria, const char* f_entrada_vigencia, const char* mes, const char* anio) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[8];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_funcion_ejecutiva(?, ?, ?, ?, ?, ?, ?, ?)";

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
    bind_param[1].buffer = (char *)cant_ur;
    bind_param[1].buffer_length = strlen(cant_ur);

    bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    bind_param[2].buffer = (char *)valor_ur;
    bind_param[2].buffer_length = strlen(valor_ur);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)monto;
    bind_param[3].buffer_length = strlen(monto);

    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = (char *)norma_regulatoria;
    bind_param[4].buffer_length = strlen(norma_regulatoria);

    bind_param[5].buffer_type = MYSQL_TYPE_STRING;
    bind_param[5].buffer = (char *)f_entrada_vigencia;
    bind_param[5].buffer_length = strlen(f_entrada_vigencia);

    bind_param[6].buffer_type = MYSQL_TYPE_STRING;
    bind_param[6].buffer = (char *)mes;
    bind_param[6].buffer_length = strlen(mes);

    bind_param[7].buffer_type = MYSQL_TYPE_STRING;
    bind_param[7].buffer = (char *)anio;
    bind_param[7].buffer_length = strlen(anio);

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


static int sp_editar_funcion_ejecutiva(int id, const char* nivel, const char* cant_ur, const char* valor_ur, const char* monto, const char* norma_regulatoria, const char* f_entrada_vigencia, const char* mes, const char* anio) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[9];
    const char *query = "CALL sp_editar_funcion_ejecutiva(?, ?, ?, ?, ?, ?, ?, ?, ?)";

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
    bind_param[2].buffer = (char *)cant_ur;
    bind_param[2].buffer_length = strlen(cant_ur);

    bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    bind_param[3].buffer = (char *)valor_ur;
    bind_param[3].buffer_length = strlen(valor_ur);

    bind_param[4].buffer_type = MYSQL_TYPE_STRING;
    bind_param[4].buffer = (char *)monto;
    bind_param[4].buffer_length = strlen(monto);

    bind_param[5].buffer_type = MYSQL_TYPE_STRING;
    bind_param[5].buffer = (char *)norma_regulatoria;
    bind_param[5].buffer_length = strlen(norma_regulatoria);

    bind_param[6].buffer_type = MYSQL_TYPE_STRING;
    bind_param[6].buffer = (char *)f_entrada_vigencia;
    bind_param[6].buffer_length = strlen(f_entrada_vigencia);

    bind_param[7].buffer_type = MYSQL_TYPE_STRING;
    bind_param[8].buffer = (char *)mes;
    bind_param[7].buffer_length = strlen(mes);

    bind_param[8].buffer_type = MYSQL_TYPE_STRING;
    bind_param[8].buffer = (char *)anio;
    bind_param[8].buffer_length = strlen(anio);

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
FuncionesEjecutivas* newFuncionEjecutiva(){

    FuncionesEjecutivas* oneFuncionEjecutiva = (FuncionesEjecutivas*)malloc(sizeof(FuncionesEjecutivas));

    if(oneFuncionEjecutiva != NULL){

        memset(oneFuncionEjecutiva, 0, sizeof(FuncionesEjecutivas));
    }

    return oneFuncionEjecutiva;

} // END OF FUNCTION


// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void funciones_ejecutivas_init_cache(ArrayList* alistFuncionesEjecutivas) {

    if(alistFuncionesEjecutivas != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListFuncionesEjecutivasLocal = alistFuncionesEjecutivas;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ Funciones Ejecutivas ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ Funciones Ejecutivas ] con NULL.\n");
    }
} // END OF FUNCTION


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void funciones_ejecutivas_load_storage(ArrayList* alistFuncionesEjecutivas) {

    if(alistFuncionesEjecutivas == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT * FROM funciones_ejecutivas");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        FuncionesEjecutivas* nFuncionEjecutiva = newFuncionEjecutiva();

        if (nFuncionEjecutiva != NULL) {
            nFuncionEjecutiva->id = atoi(row[0]);
            strncpy(nFuncionEjecutiva->nivel, row[1], sizeof(nFuncionEjecutiva->nivel) -1);
            nFuncionEjecutiva->cant_ur = atoi(row[2]);
            nFuncionEjecutiva->valor_ur = atof(row[3]);
            nFuncionEjecutiva->monto = atof(row[4]);
            strncpy(nFuncionEjecutiva->norma_regulatoria, row[5], sizeof(nFuncionEjecutiva->norma_regulatoria) -1);
            strncpy(nFuncionEjecutiva->f_entrada_vigencia, row[6], sizeof(nFuncionEjecutiva->f_entrada_vigencia) -1);
            strncpy(nFuncionEjecutiva->mes, row[7], sizeof(nFuncionEjecutiva->mes) -1);
            strncpy(nFuncionEjecutiva->anio, row[8], sizeof(nFuncionEjecutiva->anio) -1);

            alistFuncionesEjecutivas->add(alistFuncionesEjecutivas, nFuncionEjecutiva);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d FUNCIONES EJECUTIVAS cargadas. Espacio reservado: %d slots.\n", alistFuncionesEjecutivas->len(alistFuncionesEjecutivas), alistFuncionesEjecutivas->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
static void get_funcion_ejecutiva_value(const char *body, const char *key, char *out, size_t out_size) {

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
int funciones_ejecutivas_service_register(const char *body, char *error_msg, int error_size) {

    char nivel[2];
    char d_nivel[2];
    char cant_ur[11];
    char d_cant_ur[11];
    char valor_ur[11];
    char d_valor_ur[11];
    char monto[11];
    char d_monto[11];
    char norma_regulatoria[101];
    char d_norma_regulatoria[101];
    char f_entrada_vigencia[11];
    char d_f_entrada_vigencia[11];
    char mes[11];
    char d_mes[11];
    char anio[5];
    char d_anio[5];

    get_funcion_ejecutiva_value(body,"nivel", nivel, sizeof(nivel));
    url_decode(d_nivel, nivel);

    get_funcion_ejecutiva_value(body, "cant_ur", cant_ur, sizeof(cant_ur));
    url_decode(d_cant_ur, cant_ur); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "valor_ur", valor_ur, sizeof(valor_ur));
    url_decode(d_valor_ur, valor_ur); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "monto", monto, sizeof(monto));
    url_decode(d_monto, monto); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "norma_regulatoria", norma_regulatoria, sizeof(norma_regulatoria));
    url_decode(d_norma_regulatoria, norma_regulatoria); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "f_entrada_vigencia", f_entrada_vigencia, sizeof(f_entrada_vigencia));
    url_decode(d_f_entrada_vigencia, f_entrada_vigencia); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "mes", mes, sizeof(mes));
    url_decode(d_mes, mes); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "anio", anio, sizeof(anio));
    url_decode(d_anio, anio); // Asumiendo tu función de decode

    if (strlen(d_nivel) == 0 || strlen(d_cant_ur) == 0 || strlen(d_valor_ur) == 0 || strlen(d_monto) == 0 || strlen(d_norma_regulatoria) == 0 || strlen(d_f_entrada_vigencia) == 0 || strlen(d_mes) == 0 || strlen(d_anio) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListFuncionesEjecutivasLocal != NULL) {

        for (int i = 0; i < pListFuncionesEjecutivasLocal->len(pListFuncionesEjecutivasLocal); i++) {

            FuncionesEjecutivas* funciones = (FuncionesEjecutivas*) pListFuncionesEjecutivasLocal->get(pListFuncionesEjecutivasLocal, i);

            if (strcasecmp(funciones->nivel, d_nivel) == 0 && strcasecmp(funciones->norma_regulatoria, d_norma_regulatoria) == 0 && strcasecmp(funciones->f_entrada_vigencia, d_f_entrada_vigencia) == 0 && strcasecmp(funciones->mes, d_mes) == 0 && funciones->cant_ur == atoi(d_cant_ur) && funciones->valor_ur == atof(d_valor_ur) && funciones->monto == atof(d_monto)) {

                snprintf(error_msg, error_size, "Error: Función Ejecutiva existente.");
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS

    int nuevo_id = sp_insertar_funcion_ejecutiva(d_nivel, d_cant_ur, d_valor_ur, d_monto, d_norma_regulatoria, d_f_entrada_vigencia, d_mes, d_anio);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    FuncionesEjecutivas* nuevaFuncion = newFuncionEjecutiva();

    if (nuevaFuncion) {

        // Aprovechamos para inicializar el bloque de memoria limpio
        memset(nuevaFuncion, 0, sizeof(FuncionesEjecutivas));

        nuevaFuncion->id = nuevo_id;
        strncpy(nuevaFuncion->nivel, d_nivel, sizeof(nuevaFuncion->nivel) -1);
        nuevaFuncion->cant_ur = atoi(d_cant_ur);
        nuevaFuncion->valor_ur = trimDecimals(d_valor_ur, 2);
        nuevaFuncion->monto = trimDecimals(d_monto, 2);
        strncpy(nuevaFuncion->norma_regulatoria, d_norma_regulatoria, sizeof(nuevaFuncion->norma_regulatoria) -1);
        strncpy(nuevaFuncion->f_entrada_vigencia, d_f_entrada_vigencia, sizeof(nuevaFuncion->f_entrada_vigencia) -1);
        strncpy(nuevaFuncion->mes, d_mes, sizeof(nuevaFuncion->mes) -1);
        strncpy(nuevaFuncion->anio, d_anio, sizeof(nuevaFuncion->anio) -1);

        // Sincronizamos el ArrayList inmediatamente
        pListFuncionesEjecutivasLocal->add(pListFuncionesEjecutivasLocal, nuevaFuncion);

        printf("✅ Sincronización exitosa: Función Ejecutiva (ID: %d) añadida a RAM.\n", nuevo_id);
    }

    return 1;

} // END OF FUNCTION



// ===================================================================================================================================== //
// FUNCION EDICIÓN DE ACTIVIDAD
// ===================================================================================================================================== //
int funciones_ejecutivas_service_edit(const char *body, char *error_msg, int error_size) {

    char id_str[32];
    char nivel[2];
    char d_nivel[2];
    char cant_ur[11];
    char d_cant_ur[11];
    char valor_ur[11];
    char d_valor_ur[11];
    char monto[11];
    char d_monto[11];
    char norma_regulatoria[101];
    char d_norma_regulatoria[101];
    char f_entrada_vigencia[11];
    char d_f_entrada_vigencia[11];
    char mes[11];
    char d_mes[11];
    char anio[5];
    char d_anio[5];

    get_funcion_ejecutiva_value(body,"nivel", nivel, sizeof(nivel));
    url_decode(d_nivel, nivel);

    get_funcion_ejecutiva_value(body, "cant_ur", cant_ur, sizeof(cant_ur));
    url_decode(d_cant_ur, cant_ur); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "valor_ur", valor_ur, sizeof(valor_ur));
    url_decode(d_valor_ur, valor_ur); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "monto", monto, sizeof(monto));
    url_decode(d_monto, monto); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "norma_regulatoria", norma_regulatoria, sizeof(norma_regulatoria));
    url_decode(d_norma_regulatoria, norma_regulatoria); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "f_entrada_vigencia", f_entrada_vigencia, sizeof(f_entrada_vigencia));
    url_decode(d_f_entrada_vigencia, f_entrada_vigencia); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "mes", mes, sizeof(mes));
    url_decode(d_mes, mes); // Asumiendo tu función de decode

    get_funcion_ejecutiva_value(body, "anio", anio, sizeof(anio));
    url_decode(d_anio, anio); // Asumiendo tu función de decode

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_nivel) == 0 || strlen(d_cant_ur) == 0 || strlen(d_valor_ur) == 0 || strlen(d_monto) == 0 || strlen(d_norma_regulatoria) == 0 || strlen(d_f_entrada_vigencia) == 0 || strlen(d_mes) == 0 || strlen(d_anio) == 0) {
        snprintf(error_msg, error_size, "ID, o Alguno de los otros campos no contienen datos");
        return 0;
    }

    // 2. VERIFICACIÓN DE EXISTENCIA Y DUPLICADOS EN MEMORIA
    if (pListFuncionesEjecutivasLocal != NULL) {

        for (int i = 0; i < pListFuncionesEjecutivasLocal->len(pListFuncionesEjecutivasLocal); i++) {

            FuncionesEjecutivas* nFuncion = (FuncionesEjecutivas*) pListFuncionesEjecutivasLocal->get(pListFuncionesEjecutivasLocal, i);

            // Si el nombre ya existe en otro ID, rebotamos la edición
            if (nFuncion->id != id_a_editar && strcasecmp(nFuncion->nivel, d_nivel) == 0 && strcasecmp(nFuncion->norma_regulatoria, d_norma_regulatoria) == 0 && strcasecmp(nFuncion->f_entrada_vigencia, d_f_entrada_vigencia) == 0 && strcasecmp(nFuncion->mes, d_mes) == 0 && strcasecmp(nFuncion->anio, d_anio) == 0 && nFuncion->cant_ur == atoi(d_cant_ur) && nFuncion->valor_ur == trimDecimals(d_valor_ur, 2) && nFuncion->monto == trimDecimals(d_monto, 2)) {

                snprintf(error_msg, error_size, "Error: Función Ejecutiva Existente.");
                return 0;
            }
        }
    }

    // 3. ACTUALIZAR EN BASE DE DATOS (Blindado)
    if (sp_editar_funcion_ejecutiva(id_a_editar, d_nivel, d_cant_ur, d_valor_ur, d_monto, d_norma_regulatoria, d_f_entrada_vigencia, d_mes, d_anio) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 4. ACTUALIZAR EN MEMORIA (ArrayList)
    if (pListFuncionesEjecutivasLocal != NULL) {

        for (int i = 0; i < pListFuncionesEjecutivasLocal->len(pListFuncionesEjecutivasLocal); i++) {

            FuncionesEjecutivas* nFuncion = (FuncionesEjecutivas*) pListFuncionesEjecutivasLocal->get(pListFuncionesEjecutivasLocal, i);

            if (nFuncion->id == id_a_editar) {
                // Actualizamos el puntero directamente en la memoria
                strncpy(nFuncion->nivel, d_nivel, sizeof(nFuncion->nivel) -1);
                nFuncion->cant_ur = atoi(d_cant_ur);
                nFuncion->valor_ur = trimDecimals(d_valor_ur, 2);
                nFuncion->monto = trimDecimals(d_monto, 2);
                strncpy(nFuncion->norma_regulatoria, d_norma_regulatoria, sizeof(nFuncion->norma_regulatoria) -1);
                strncpy(nFuncion->f_entrada_vigencia, d_f_entrada_vigencia, sizeof(nFuncion->f_entrada_vigencia) -1);
                strncpy(nFuncion->mes, d_mes, sizeof(nFuncion->mes) -1);
                strncpy(nFuncion->anio, d_anio, sizeof(nFuncion->anio) -1);
                printf("✅ Memoria sincronizada: Funzión Ejecutiva ID %d actualizada.\n", id_a_editar);
                break;
            }
        }
    }

    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_funcion_ejecutiva_service_id(const char *body, char *json_out, int out_size) {

    char id_str[10];
    get_funcion_ejecutiva_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512];

    snprintf(query, sizeof(query),
             "SELECT * FROM funciones_ejecutivas WHERE id = %s LIMIT 1;", id_str);

    DBResult *res = db_query(query);

    // 🔹 CORRECCIÓN AQUÍ: Usamos db_fetch_row o la función correspondiente de tu db.h
    // Si tu db.h usa mysql_fetch_row directamente:
    MYSQL_ROW row;

    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Función Ejecutiva no encontrada\" }");
        if (res) db_free_result(res);
        return 0;
    }

    // Construimos el JSON usando los índices del array 'row'
    snprintf(json_out, out_size,
             "{ \"id\": %s, \"nivel\": \"%s\", \"cant_ur\": \"%s\", \"valor_ur\": \"%s\", \"monto\": \"%s\", \"norma_regulatoria\": \"%s\", \"f_entrada_vigencia\": \"%s\", \"mes\": \"%s\", \"anio\": \"%s\" }",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "",
             row[2] ? row[2] : "0",
             row[3] ? row[3] : "0",
             row[4] ? row[4] : "0",
             row[5] ? row[5] : "0",
             row[6] ? row[6] : "",
             row[7] ? row[7] : "",
             row[8] ? row[8] : "");

    db_free_result(res);
    return 1;

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_funciones_ejecutivas(int client, const char *body) {

    char error_msg[256];

    if (funciones_ejecutivas_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Función Ejecutiva creada y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// Handler POST para la edición
// ===================================================================================================================================== //
static void route_post_funciones_ejecutivas_edit(int client, const char *body) {

    char error_msg[256];

    if (funciones_ejecutivas_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Función Ejecutiva actualizada correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }

} // END OF FUNCTION


// ===================================================================================================================================== //
// ROUTES FOR LIST FUNCIONES EJECUTIVAS
// ===================================================================================================================================== //
static void route_get_funciones_ejecutivas_list(int client, const char *body) {


    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListFuncionesEjecutivasLocal == NULL) {
        printf("❌ Error crítico: pListFuncionesEjecutivasLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    // Estimamos el tamaño del JSON (aprox 150 bytes por médico)
    size_t total_registros = pListFuncionesEjecutivasLocal->len(pListFuncionesEjecutivasLocal);
    size_t buffer_size = (total_registros * 650) + 512;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_registros; i++) {

        FuncionesEjecutivas* oneFuncion = (FuncionesEjecutivas*) pListFuncionesEjecutivasLocal->get(pListFuncionesEjecutivasLocal, i);

        char item[600];

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"nivel\": \"%s\" , \"cant_ur\": \"%d\", \"valor_ur\": \"%.2f\", \"monto\": \"%.2f\", \"norma_regulatoria\": \"%s\", \"f_entrada_vigencia\": \"%s\", \"mes\": \"%s\", \"anio\": \"%s\" }%s",
            oneFuncion->id, oneFuncion->nivel, oneFuncion->cant_ur, oneFuncion->valor_ur, oneFuncion->monto, oneFuncion->norma_regulatoria, oneFuncion->f_entrada_vigencia, oneFuncion->mes, oneFuncion->anio, (i < total_registros - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON


} // END OF FUNCTION
