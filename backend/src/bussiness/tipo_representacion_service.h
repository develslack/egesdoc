#ifndef TIPO_REPRESENTACION_SERVICE_H
#define TIPO_REPRESENTACION_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char descripcion[101];

}TipoRepresentacion;

TipoRepresentacion* newTipoRepresentacion();
void tipo_representacion_init_cache(ArrayList* alistTipoRepresentacion);
void tipo_representacion_load_storage(ArrayList* alistTipoRepresentacion);


int tipo_representacion_service_register(const char *body, char *error_msg, int error_size);
int tipo_representacion_service_edit(const char *body, char *error_msg, int error_size);
int get_tipo_representacion_service_id(const char *body, char *json_out, int out_size);

void init_tipo_representacion_routes();


#endif



