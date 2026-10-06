#ifndef FUNCIONES_EJECUTIVAS_SERVICE_H
#define FUNCIONES_EJECUTIVAS_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char  nivel[2];
    int   cant_ur;
    float valor_ur;
    float monto;
    char  norma_regulatoria[101];
    char  f_entrada_vigencia[11];
    char  mes[11];
    char  anio[5];

}FuncionesEjecutivas;

FuncionesEjecutivas* newFuncionEjecutiva();
void funciones_ejecutivas_init_cache(ArrayList* alistFuncionesEjecutivas);
void funciones_ejecutivas_load_storage(ArrayList* alistFuncionesEjecutivas);


int funciones_ejecutivas_service_register(const char *body, char *error_msg, int error_size);
int funciones_ejecutivas_service_edit(const char *body, char *error_msg, int error_size);
int get_funcion_ejecutiva_service_id(const char *body, char *json_out, int out_size);

void init_funciones_ejecutivas_routes();


#endif
