#include <Esp32_Utils.hpp>
#include <Server.hpp>

void setup() {
  Serial.begin(9600);

  // Intentar conectarse al WiFi almacenado
  conectarseAWifi();
}

void loop() {
  // Si está en modo AP, maneja las solicitudes HTTP
  if (WiFi.getMode() == WIFI_AP) {
    server.handleClient();  // Manejar solicitudes HTTP en modo AP
  }
}