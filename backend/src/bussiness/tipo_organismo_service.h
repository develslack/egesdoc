#ifndef TIPO_ORGANISMO_SERVICE_H
#define TIPO_ORGANISMO_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char cod_organismo[3];
    char descripcion[121];

}TipoOrganismo;

TipoOrganismo* newTipoOrganismo();
void tipo_organismo_init_cache(ArrayList* alisttipoOrganismo);
void tipo_organismo_load_storage(ArrayList* alisttipoOrganismo);


int tipo_organismo_service_register(const char *body, char *error_msg, int error_size);
int tipo_organismo_service_edit(const char *body, char *error_msg, int error_size);
int get_tipo_organismo_service_id(const char *body, char *json_out, int out_size);

void init_tipo_organismo_routes();


#endif

