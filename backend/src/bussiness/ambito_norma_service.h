#ifndef AMBITO_NORMA_SERVICE_H
#define AMBITO_NORMA_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char descripcion[101];

}AmbitoNorma;

AmbitoNorma* newAmbitoNorma();
void ambito_norma_init_cache(ArrayList* alistAmbitoNorma);
void ambito_norma_load_storage(ArrayList* alistAmbitoNorma);


int ambito_norma_service_register(const char *body, char *error_msg, int error_size);
int ambito_norma_service_edit(const char *body, char *error_msg, int error_size);
int get_ambito_norma_service_id(const char *body, char *json_out, int out_size);

void init_ambito_norma_routes();


#endif
