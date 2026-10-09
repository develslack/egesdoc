#ifndef JURISDICCIONES_SERVICE_H
#define JURISDICCIONES_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char cod_jur[3];
    char descripcion[91];

}Jurisdicciones;

Jurisdicciones* newJurisdiccion();
void jurisdicciones_init_cache(ArrayList* alistJurisdicciones);
void jurisdicciones_load_storage(ArrayList* alistJurisdicciones);


int jurisdicciones_service_register(const char *body, char *error_msg, int error_size);
int jurisdicciones_service_edit(const char *body, char *error_msg, int error_size);
int get_jurisdiccion_service_id(const char *body, char *json_out, int out_size);

void init_jurisdicciones_routes();


#endif


