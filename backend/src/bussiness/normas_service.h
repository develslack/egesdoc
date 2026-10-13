#ifndef NORMAS_SERVICE_H
#define NORMAS_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"


typedef struct{

    int  id;
    char nombre_norma[501];
    char n_norma[101];
    char tipo_norma[201];
    char f_norma[201];
    char f_pub[11];
    char anio_pub[5];
    char jurisdiccion[101];
    char organismo[101];
    char unidad_fisica[101];
    char observaciones[3000];
    char file_name[101];
    char file_path[101];
    char descripcion_organismo[100];

}Normas;

Normas* newNorma();

void normas_init_cache(ArrayList* alistNormas);
void normas_load_storage(ArrayList* alistNormas);

int normas_service_register(const char *body, char *error_msg, int error_size);
int normas_service_edit(const char *body, char *error_msg, int error_size);
int get_norma_service_id(const char *body, char *json_out, int out_size);

void init_normas_routes();


#endif
