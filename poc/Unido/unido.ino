#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Adafruit_SHTC3.h>
#include <HTTPClient.h>

// Variables globales
Preferences preferences;
WebServer server(80);
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

// Estados
bool wifiConectado = false;

// Configuración de Wi-Fi
const char* AP_SSID = "ESP32C3-Config";
const char* AP_PASSWORD = "12345678";

// Dirección del servidor
const char* SERVER_URL = "http://192.168.1.128:1880";

void setup() {
  Serial.begin(9600);
  if (!shtc3.begin()) {
    Serial.println("No se pudo encontrar el sensor SHTC3.");
    while (1);
  }

  // Cargar credenciales Wi-Fi de NVS
  preferences.begin("wifi", false);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");

  if (ssid != "" && password != "") {
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Conectando a Wi-Fi");
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
      delay(1000);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConectado a Wi-Fi!");
      wifiConectado = true;
    }
  }

  if (!wifiConectado) {
    iniciarModoAP();
  }
}

void loop() {
  if (!wifiConectado) {
    server.handleClient();
  } else {
    float temp, hum;
    if (leerSensor(temp, hum)) {
      enviarDatosServidor(temp, hum);
    }
    delay(60000); // Esperar 1 minuto antes de la siguiente lectura
  }
}

// Modo Access Point
void iniciarModoAP() {
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.println("Modo AP iniciado");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Dirección IP: ");
  Serial.println(IP);

  // Configurar servidor web
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", R"rawliteral(
      <form action="/guardar" method="post">
        SSID: <input type="text" name="ssid"><br>
        Password: <input type="password" name="password"><br>
        <input type="submit" value="Guardar">
      </form>
    )rawliteral");
  });

  server.on("/guardar", HTTP_POST, []() {
    if (server.hasArg("ssid") && server.hasArg("password")) {
      String ssid = server.arg("ssid");
      String password = server.arg("password");

      preferences.putString("ssid", ssid);
      preferences.putString("password", password);

      server.send(200, "text/plain", "Credenciales guardadas. Reinicie el dispositivo.");

      delay(1000);
      ESP.restart();
    } else {
      server.send(400, "text/plain", "Faltan parámetros.");
    }
  });

  server.begin();
}

// Leer sensor
bool leerSensor(float &temp, float &hum) {
  sensors_event_t humidity, tempEvent;
  if (shtc3.getEvent(&humidity, &tempEvent)) {
    temp = tempEvent.temperature;
    hum = humidity.relative_humidity;
    Serial.printf("Temp: %.2f ºC, Hum: %.2f %%\n", temp, hum);
    return true;
  }
  Serial.println("Error al leer el sensor.");
  return false;
}

// Enviar datos al servidor
void enviarDatosServidor(float temp, float hum) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Obtener la dirección MAC
    String macAddress = WiFi.macAddress();

    String payload = String("{\"mac\":\"") + macAddress + 
                     String("\",\"temperature\":") + temp + 
                     String(",\"humidity\":") + hum + String("}");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.printf("Datos enviados: %s\n", payload.c_str());
      Serial.printf("Respuesta del servidor: %d\n", httpResponseCode);
    } else {
      Serial.printf("Error al enviar datos: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado, no se pueden enviar datos.");
  }
}