#ifndef NORMAS_VINCULADAS_SERVICE_H
#define NORMAS_VINCULADAS_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

#define PATH_MAX 300


typedef struct{

    int  id;
    int  id_norma_principal;
    char path_name[101];

}NormasVinculadas;

NormasVinculadas* newNormasVinculadas();

void normas_vinculadas_init_cache(ArrayList* alistNormasVinculadas);
void normas_vinculadas_load_storage(ArrayList* alistNormasVinculadas);

int normas_vinculadas_service_register(const char *body, char *error_msg, int error_size);
int get_informacion_extendida_service(const char *body, char *json_out, int out_size);

void init_normas_vinculadas_routes();


#endif

