#ifndef SERVER_H
#define SERVER_H

#include <WebServer.h>
#include "config.h" // Asegúrate de incluir config.h

extern WebServer server; // Declaración externa del servidor web

// Función para manejar la página principal del servidor
void handleRoot();

// Función para manejar la solicitud de guardar las credenciales Wi-Fi
void handleSave();

#endif // SERVER_H