#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "system/system_struct.h"
#include "system/frontend_server.h"
#include "system/backend_server.h"
#include "system/session_manager.h"
#include "system/ArrayList.h"
#include "bussiness/roles_service.h"
#include "bussiness/funciones_ejecutivas_service.h"
#include "bussiness/adicional_grado_service.h"
#include "bussiness/unidades_retributivas_service.h"
#include "bussiness/tipo_organismo_service.h"
#include "bussiness/tipo_norma_service.h"






int main() {

    // 1. Limpieza visual de la terminal de Slackware/Debian
    system("clear");
    printf("=== AUDITORÍA DE INFRAESTRUCTURA DE SISTEMA (FAIL-FAST) ===\n");

    // 2. Activamos el blindaje de sockets primero
    system_blindar_senales();

    // 3. Freno de mano: Si hay menos de 4 núcleos, esta función imprime el error,
    // escribe en el log y ejecuta un exit(EXIT_FAILURE), frenando el main ACÁ mismo.
    system_verificar_y_mapear_hardware();

    // 4. Iniciamos session_manager como controlador del sistema
    session_manager_init();


    // ================================================================================= //
    // 🚀 SI LLEGA A ESTE PUNTO, EL HARDWARE ESTÁ BLINDADO Y ES APTO PARA 24x365
    // ================================================================================= //
    printf("\n=== INICIANDO CONFIGURACIÓN DE MEMORIA DEL MAESTRO ===\n");

    // 1. SE CREA LOS ARRAYLIST
    //ArrayList* alist_roles = al_newArrayList();
    //roles_init_cache(alist_roles);
    //roles_load_storage(alist_roles);

    // FUNCIONES EJECUTIVAS
    ArrayList* alist_funciones_ejecutivas = al_newArrayList();
    funciones_ejecutivas_init_cache(alist_funciones_ejecutivas);
    funciones_ejecutivas_load_storage(alist_funciones_ejecutivas);

    // ADICIONAL GRADO
    ArrayList* alist_adicional_grado = al_newArrayList();
    adicional_grado_init_cache(alist_adicional_grado);
    adicional_grado_load_storage(alist_adicional_grado);

    // UNIDADES RETRIBUTIVAS
    ArrayList* alist_unidades_retributivas = al_newArrayList();
    unidades_retributivas_init_cache(alist_unidades_retributivas);
    unidades_retributivas_load_storage(alist_unidades_retributivas);

    // TIPO ORGANISMO
    ArrayList* alist_tipo_organismo = al_newArrayList();
    tipo_organismo_init_cache(alist_tipo_organismo);
    tipo_organismo_load_storage(alist_tipo_organismo);

    // TIPO NORMA
    ArrayList* alist_tipo_norma = al_newArrayList();
    tipo_norma_init_cache(alist_tipo_norma);
    tipo_norma_load_storage(alist_tipo_norma);



    // 40. INCIALAR CACHE DE SESSION MANAGER (MODULAR)
    session_manager_init();

    printf("Iniciando aplicación...\n");

    // 🔹 Levantar frontend
    start_frontend_server(4001, "../../frontend/templates/views");
    //  └── dentro de ./frontend deben estar index.html y dashboard.html

    // 🔹 Luego iniciar backend normalmente
    start_backend(4000);

    return 0;
}


