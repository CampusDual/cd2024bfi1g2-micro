#include <WebServer.h>
#include <Preferences.h>
#include "Server.hpp"

// Definir las variables globales
WebServer server(80);
Preferences preferences;

// Función para manejar la página de configuración
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