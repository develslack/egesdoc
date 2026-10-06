#ifndef ADICIONAL_GRADO_SERVICE_H
#define ADICIONAL_GRADO_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char  nivel[2];
    char  grado[3];
    int   cant_ur;

}AdicionalGrado;

AdicionalGrado* newAdicionalGrado();
void adicional_grado_init_cache(ArrayList* alistAdicionalGrado);
void adicional_grado_load_storage(ArrayList* alistAdicionalGrado);


int adicional_grado_service_register(const char *body, char *error_msg, int error_size);
int adicional_grado_service_edit(const char *body, char *error_msg, int error_size);
int get_adicional_grado_service_id(const char *body, char *json_out, int out_size);

void init_adicional_grado_routes();


#endif

