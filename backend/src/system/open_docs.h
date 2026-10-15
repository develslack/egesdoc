#ifndef OPEN_DOCS_H
#define OPEN_DOCS_H

/**
 * Abre un archivo físico en el servidor y lo transmite por el socket HTTP al cliente.
 *
 * @param client Descriptor del socket del cliente HTTP.
 * @param absolute_file_path Ruta física real del archivo en el servidor (ej. /var/www/uploads/doc.pdf).
 * @param original_file_name Nombre original del archivo para indicarle al navegador el tipo y nombre.
 */
void serve_document(int client, const char *absolute_file_path, const char *original_file_name);

#endif // OPEN_DOCS_H
