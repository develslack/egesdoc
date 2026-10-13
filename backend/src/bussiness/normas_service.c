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
#include "normas_service.h"

// ===================================================================================================================================== //

// Variable estática para la memoria local del módulo
static ArrayList* pListNormasLocal = NULL;


// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (NORMAS)
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

// ===================================================================================================================================== //


// ===================================================================================================================================== //


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

// ===================================================================================================================================== //
// CONSTRUCTOR
// ===================================================================================================================================== //
Normas* newNorma(){

    Normas* nNorma = (Normas*)malloc(sizeof(Normas));

    if(nNorma != NULL){

        memset(nNorma, 0, sizeof(Normas));
    }

    return nNorma;

} // END OF FUNCTION


// ===================================================================================================================================== //
// Iniciliacion de cache
// ===================================================================================================================================== //
void normas_init_cache(ArrayList* alistNormas) {

    if(alistNormas != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListNormasLocal = alistNormas;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ NORMAS ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ NORMAS ] con NULL.\n");
    }
} // END OF FUNCTION


// ===================================================================================================================================== //
// cargar datos de permisos en ArrayList
// ===================================================================================================================================== //
void normas_load_storage(ArrayList* alistNormas) {

    if(alistNormas == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("SELECT n.*, o.descripcion FROM normas n LEFT JOIN organismos o ON n.organismo = o.cod_org");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {

        Normas* nNorma= newNorma();

        if (nNorma != NULL) {
            nNorma->id = atoi(row[0]);
            strncpy(nNorma->nombre_norma, row[1], sizeof(nNorma->nombre_norma) -1);
            strncpy(nNorma->n_norma, row[2], sizeof(nNorma->n_norma) -1);
            strncpy(nNorma->tipo_norma, row[3], sizeof(nNorma->tipo_norma) -1);
            strncpy(nNorma->f_norma, row[4], sizeof(nNorma->f_norma) -1);
            strncpy(nNorma->f_pub, row[5], sizeof(nNorma->f_pub) -1);
            strncpy(nNorma->anio_pub, row[6], sizeof(nNorma->anio_pub) -1);
            strncpy(nNorma->jurisdiccion, row[7], sizeof(nNorma->jurisdiccion) -1);
            strncpy(nNorma->organismo, row[8], sizeof(nNorma->organismo) -1);
            strncpy(nNorma->unidad_fisica, row[9], sizeof(nNorma->unidad_fisica) -1);
            strncpy(nNorma->observaciones, row[10], sizeof(nNorma->observaciones) -1);
            strncpy(nNorma->file_name, row[11], sizeof(nNorma->file_name) -1);
            strncpy(nNorma->file_path, row[12], sizeof(nNorma->file_path) -1);
            strncpy(nNorma->descripcion_organismo, row[13], sizeof(nNorma->descripcion_organismo) -1);


            alistNormas->add(alistNormas, nNorma);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d NORMAS cargadas. Espacio reservado: %d slots.\n", alistNormas->len(alistNormas), alistNormas->reservedSize);

} // END OF FUNCTION


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
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

} // END OF FUNCTION


// ===================================================================================================================================== //
// FUNCION PARA REGISTRAR NUEVA FUNCION EJECUTIVA
// ===================================================================================================================================== //
int normas_service_register(const char *body, char *error_msg, int error_size) {

    char nombre_norma[501];
    char d_nombre_norma[501];

    char n_norma[101];
    char d_n_norma[101];

    char tipo_norma[201];
    char d_tipo_norma[201];

    char f_norma[201];
    char d_f_norma[201];

    char f_pub[11];
    char d_f_pub[11];

    char anio_pub[5];
    char d_anio_pub[5];

    char jurisdiccion[101];
    char d_jurisdiccion[101];

    char organismo[101];
    char d_organismo[101];

    char unidad_fisica[101];
    char d_unidad_fisica[101];

    char observaciones[3000];
    char d_observaciones[3000];

    char file_name[101];
    char d_file_name[101];

    char file_path[101];
    char d_file_path[101];

    get_norma_value(body,"nombre_norma", nombre_norma, sizeof(nombre_norma));
    url_decode(d_nombre_norma, nombre_norma);

    get_norma_value(body, "n_norma", n_norma, sizeof(n_norma));
    url_decode(d_n_norma, n_norma); // Asumiendo tu función de decode

    get_norma_value(body, "tipo_norma", tipo_norma, sizeof(tipo_norma));
    url_decode(d_tipo_norma, tipo_norma); // Asumiendo tu función de decode

    get_norma_value(body, "f_norma", f_norma, sizeof(f_norma));
    url_decode(d_f_norma, f_norma); // Asumiendo tu función de decode

    get_norma_value(body, "f_pub", f_pub, sizeof(f_pub));
    url_decode(d_f_pub, f_pub); // Asumiendo tu función de decode

    get_norma_value(body, "anio_pub", anio_pub, sizeof(anio_pub));
    url_decode(d_anio_pub, anio_pub); // Asumiendo tu función de decode

    get_norma_value(body, "jurisdiccion", jurisdiccion, sizeof(jurisdiccion));
    url_decode(d_jurisdiccion, jurisdiccion); // Asumiendo tu función de decode

    get_norma_value(body, "organismo", organismo, sizeof(organismo));
    url_decode(d_organismo, organismo); // Asumiendo tu función de decode

    get_norma_value(body, "unidad_fisica", unidad_fisica, sizeof(unidad_fisica));
    url_decode(d_unidad_fisica, unidad_fisica); // Asumiendo tu función de decode

    get_norma_value(body, "observaciones", observaciones, sizeof(observaciones));
    url_decode(d_observaciones, observaciones); // Asumiendo tu función de decode

    get_norma_value(body, "file_name", file_name, sizeof(file_name));
    url_decode(d_file_name, file_name); // Asumiendo tu función de decode

    get_norma_value(body, "file_path", file_path, sizeof(file_path));
    url_decode(d_file_path, file_path); // Asumiendo tu función de decode

    if (strlen(d_nombre_norma) == 0 || strlen(d_n_norma) == 0 || strlen(d_tipo_norma) == 0 || strlen(d_f_norma) == 0 || strlen(d_f_pub) == 0 || strlen(d_anio_pub) == 0 || strlen(d_jurisdiccion) == 0 || strlen(d_organismo) == 0 || strlen(d_unidad_fisica) == 0 || strlen(d_observaciones) == 0 || strlen(d_file_name) == 0 || strlen(d_file_path) == 0){
        snprintf(error_msg, error_size, "Hay Campos sin Completar.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListNormasLocal != NULL) {

        for (int i = 0; i < pListNormasLocal->len(pListNormasLocal); i++) {

            Normas* nNorma = (Normas*) pListNormasLocal->get(pListNormasLocal, i);

            if (strcasecmp(nNorma->nombre_norma, d_nombre_norma) == 0
                && strcasecmp(nNorma->n_norma, d_n_norma) == 0
                && strcasecmp(nNorma->tipo_norma, d_tipo_norma) == 0
                && strcasecmp(nNorma->f_norma, d_f_norma) == 0
                && strcasecmp(nNorma->f_pub, d_f_pub) == 0
                && strcasecmp(nNorma->anio_pub, d_anio_pub) == 0
                && strcasecmp(nNorma->jurisdiccion, d_jurisdiccion) == 0
                && strcasecmp(nNorma->organismo, d_organismo) == 0
                && strcasecmp(nNorma->unidad_fisica, d_unidad_fisica) == 0
                && strcasecmp(nNorma->observaciones, d_observaciones) == 0
                && strcasecmp(nNorma->file_name, d_file_name) == 0
                && strcasecmp(nNorma->file_path, d_file_path) == 0) {

                snprintf(error_msg, error_size, "Error: Norma Existente.");
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS

    int nuevo_id = sp_insertar_normas(d_nombre_norma, d_n_norma, d_tipo_norma, d_f_norma, d_f_pub, d_anio_pub, d_jurisdiccion, d_organismo, d_unidad_fisica, d_observaciones, d_file_name, d_file_path);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    Normas* nuevaNorma = newNorma();

    if (nuevaNorma) {

        // Aprovechamos para inicializar el bloque de memoria limpio
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


        // Sincronizamos el ArrayList inmediatamente
        pListNormasLocal->add(pListNormasLocal, nuevaNorma);

        printf("✅ Sincronización exitosa: NORMA (ID: %d) añadida a RAM.\n", nuevo_id);
    }

    return 1;

} // END OF FUNCTION
