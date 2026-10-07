#ifndef UNIDADES_RETRIBUTIVAS_SERVICE_H
#define UNIDADES_RETRIBUTIVAS_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../system/ArrayList.h"

typedef struct{

    int   id;
    char  nivel[2];
    char  grado[3];
    int   sueldo_ur;
    int   dedicacion_funcional_ur;
    int   total_ur;

}UnidadesRetributivas;

UnidadesRetributivas* newUnidadRetributiva();
void unidades_retributivas_init_cache(ArrayList* alistUnidadesRetributivas);
void unidades_retributivas_load_storage(ArrayList* alistUnidadesRetributivas);


int unidades_retributivas_service_register(const char *body, char *error_msg, int error_size);
int unidades_retributivas_service_edit(const char *body, char *error_msg, int error_size);
int get_unidades_retributivas_service_id(const char *body, char *json_out, int out_size);

void init_unidades_retributivas_routes();


#endif
