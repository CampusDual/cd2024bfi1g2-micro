#include "config.h" // Incluir el archivo de configuración para acceder a las variables ssid, password y serverUrl
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>

// Función para conectarse al Wi-Fi
bool connectToWiFi() {
  WiFi.begin(ssid, password); // Usa las credenciales de config.h
  Serial.print("Conectando a WiFi");

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 10) {
    delay(1000);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("¡Conectado!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("No se pudo conectar al Wi-Fi.");
    return false;
  }
}

// Función para enviar datos de los sensores al servidor
void sendSensorData(float humidity, float temperature) {
  HTTPClient http;
  http.begin(serverUrl);  // Usa la URL del servidor desde config.h

  http.addHeader("Content-Type", "application/json");

  String payload = "{\"humidity\": " + String(humidity) + ", \"temperature\": " + String(temperature) + "}";
  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.println("Datos enviados correctamente.");
  } else {
    Serial.println("Error al enviar los datos.");
  }

  http.end();
}