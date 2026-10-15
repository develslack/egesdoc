#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "open_docs.h"
#include "routes.h" // Asumiendo que aquí tienes tu función send_response para errores

void serve_document(int client, const char *absolute_file_path, const char *original_file_name) {

    // 1. Abrir el archivo físico en modo lectura binaria ("rb")
    FILE *f = fopen(absolute_file_path, "rb");
    if (!f) {
        send_response(client, "404 Not Found", "application/json", "{\"error\": \"El archivo físico no se encuentra en el servidor\"}");
        return;
    }

    // 2. Determinar el tamaño total del archivo en bytes
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // 3. Determinar el Content-Type (MIME type) dinámicamente según la extensión
    const char *mime_type = "application/octet-stream"; // Genérico por defecto
    if (strstr(original_file_name, ".pdf")) {
        mime_type = "application/pdf";
    } else if (strstr(original_file_name, ".doc") || strstr(original_file_name, ".docx")) {
        mime_type = "application/msword";
    } else if (strstr(original_file_name, ".txt")) {
        mime_type = "text/plain";
    } else if (strstr(original_file_name, ".jpg") || strstr(original_file_name, ".jpeg")) {
        mime_type = "image/jpeg";
    } else if (strstr(original_file_name, ".png")) {
        mime_type = "image/png";
    }

    // 4. Escribir y enviar cabeceras HTTP crudas al socket del cliente.
    char header[1024];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Content-Disposition: inline; filename=\"%s\"\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "Connection: close\r\n\r\n",
             mime_type, fsize, original_file_name);

    write(client, header, strlen(header));

    // 5. Leer el archivo binario en bloques de 8KB y enviarlo directamente por el socket
    char buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        write(client, buffer, bytes_read);
    }

    fclose(f);
    printf("📄 [VISOR UNIVERSAL] Documento servido al cliente: %s (%ld bytes)\n", original_file_name, fsize);
}
