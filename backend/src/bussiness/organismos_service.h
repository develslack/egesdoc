#ifndef ORGANISMOS_SERVICE_H
#define ORGANISMOS_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char  cod_org[3];
    char  saf[5];
    char  descripcion[301];
    char  ubicacion_fisica[121];

}Organismos;

Organismos* newOrganismo();
void organismos_init_cache(ArrayList* alistOrganismos);
void organismos_load_storage(ArrayList* alistOrganismos);


int organismos_service_register(const char *body, char *error_msg, int error_size);
int organismos_service_edit(const char *body, char *error_msg, int error_size);
int get_organismo_service_id(const char *body, char *json_out, int out_size);

void init_organismos_routes();


#endif

