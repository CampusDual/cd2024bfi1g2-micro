#include "Server.hpp"
#include <Preferences.h> // Incluir la librería Preferences
#include "config.h"

// Definición de las variables
const char* ssid = "Tu_SSID";         // Cambia esto por tu SSID
const char* password = "Tu_Contraseña"; // Cambia esto por tu contraseña
const char* serverUrl = "http://direccion.del.servidor.com"; // Cambia esto por la URL de tu servidor

Preferences preferences; // Declarar preferencias globalmente
WebServer server(80);    // Definición del servidor en el puerto 80

// Función para manejar la página principal del servidor
void handleRoot() {
  String html = R"rawliteral(
    <form action="/save" method="POST">
      SSID: <input type="text" name="ssid"><br>
      Contraseña: <input type="password" name="password"><br><br>
      <input type="submit" value="Guardar">
    </form>
  )rawliteral";
  
  server.send(200, "text/html", html);
}

// Función para manejar la solicitud de guardar las credenciales Wi-Fi
void handleSave() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  if (ssid.isEmpty() || password.isEmpty()) {
    server.send(400, "text/plain", "Por favor, completa ambos campos.");
    return;
  }

  // Guardar en memoria no volátil
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();

  server.send(200, "text/html", "Credenciales guardadas. Reinicia el dispositivo.");
}