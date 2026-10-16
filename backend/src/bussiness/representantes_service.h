#ifndef REPRESENTANTES_SERVICE_H
#define REPRESENTANTES_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char  nombre_representante[101];
    char  dni_representante[11];
    char  email_representante[51];

}Representantes;

Representantes* newRepresentante();
void representantes_init_cache(ArrayList* alistRepresentantes);
void representantes_load_storage(ArrayList* alistRepresentantes);


int representantes_service_register(const char *body, char *error_msg, int error_size);
int representantes_service_edit(const char *body, char *error_msg, int error_size);
int get_representantes_service_id(const char *body, char *json_out, int out_size);

void init_representantes_routes();


#endif


