#ifndef SERVER_H
#define SERVER_H

#include <WebServer.h>
#include <Preferences.h>

// Declaración externa de variables globales
extern WebServer server;
extern Preferences preferences;

// Función para manejar la página de configuración
void handleRoot();

#endif