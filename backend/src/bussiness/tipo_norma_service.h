#ifndef TIPO_NORMA_SERVICE_H
#define TIPO_NORMA_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char descripcion[101];

}TipoNorma;

TipoNorma* newNorma();
void tipo_norma_init_cache(ArrayList* alistTipoNorma);
void tipo_norma_load_storage(ArrayList* alistTipoNorma);


int tipo_norma_service_register(const char *body, char *error_msg, int error_size);
int tipo_norma_service_edit(const char *body, char *error_msg, int error_size);
int get_tipo_norma_service_id(const char *body, char *json_out, int out_size);

void init_tipo_norma_routes();


#endif


