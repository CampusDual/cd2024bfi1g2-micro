#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <config.h>
#include <Esp32_Utils.hpp>
#include <Server.hpp>


handleRoot();
handleSave();
connectToWiFi();
void setup() {
  Serial.begin(9600);

  // Intentar conectarse al WiFi almacenado
  conectarseAWifi();
}

void loop() {
  if (WiFi.getMode() == WIFI_AP) {
    server.handleClient();  // Manejar solicitudes HTTP en modo AP
  }
}