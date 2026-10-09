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

}Normas;



#endif
