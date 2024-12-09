#include <WiFi.h>
#include <ESP32_Utils.hpp>
#include <Server.hpp>
#include <Preferences.h>

// Instancia del sensor SHTC3
#include <Wire.h>
#include <Adafruit_SHTC3.h>
Adafruit_SHTC3 shtc3;

void setup() {
  Serial.begin(115200);  // Iniciar la comunicación serial
  Serial.println("Iniciando...");

  Wire.begin();

  if (!shtc3.begin()) {
    Serial.println("No se pudo encontrar el sensor SHTC3");
    while (1);
  }

  // Intentar conectarse al Wi-Fi almacenado
  if (!connectToWiFi()) {
    // Si no se puede conectar al Wi-Fi, inicia el modo AP
    Serial.println("Iniciando en modo AP...");
    WiFi.softAP("ESP32_Config", "12345678");
    Serial.print("IP del AP: ");
    Serial.println(WiFi.softAPIP());

    // Iniciar el servidor web
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
    Serial.println("Servidor web iniciado.");
  }
}

void loop() {
  // Manejar solicitudes HTTP si está en modo AP
  if (WiFi.getMode() == WIFI_AP) {
    server.handleClient();  // Manejar las solicitudes HTTP
  }

  // Lógica para leer los sensores y enviar datos al servidor remoto
  float temperature, humidity;

  sensors_event_t sensorHumidity, sensorTemperature;
  shtc3.getEvent(&sensorHumidity, &sensorTemperature);
  
  if (isnan(sensorHumidity.relative_humidity) || isnan(sensorTemperature.temperature)) {
    Serial.println("Error al leer del sensor!");
    return;
  }

  humidity = sensorHumidity.relative_humidity;
  temperature = sensorTemperature.temperature;

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" *C");

  sendSensorData(humidity, temperature); // Enviar los datos al servidor
  delay(60000); // Esperar 60 segundos antes de enviar nuevos datos
}