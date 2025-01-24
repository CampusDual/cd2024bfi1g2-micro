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

void setup() {
    Serial.begin(9600);

    if (!shtc3.begin()) {
        Serial.println("No se pudo encontrar el sensor SHTC3.");
        while (1);
    }

    // Cargar credenciales Wi-Fi y URL del servidor de NVS
    preferences.begin("wifi", false);
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");
    String serverURL = preferences.getString("serverURL", "");

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
        delay(20000); // Esperar 20 segundos antes de la siguiente lectura
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
                Server URL: <input type="text" name="serverURL"><br>
                <input type="submit" value="Guardar">
            </form>
        )rawliteral");
    });

    server.on("/guardar", HTTP_POST, []() {
        if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("serverURL")) {
            String ssid = server.arg("ssid");
            String password = server.arg("password");
          //  String serverURL = server.arg("serverURL");

            preferences.putString("ssid", ssid);
            preferences.putString("password", password);
            preferences.putString("serverURL", serverURL);

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
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi desconectado, reconectando...");
        WiFi.reconnect();
        delay(1000);
        return;
    }

    HTTPClient http;

    // Cargar la URL del servidor de la memoria NVS
    String serverURL = preferences.getString("serverURL", "");
    if (serverURL == "") {
        Serial.println("URL del servidor no configurada.");
        return;
    }

    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Obtener la dirección MAC
    String macAddress = WiFi.macAddress();

     // Crear el JSON con los datos
    String payload = "{\"data\": {";
           payload += "\"DEV_MAC\": \"" + macAddress + "\",";
           payload += "\"ME_TEMP\": " + String(temp) + ",";
           payload += "\"ME_HUMIDITY\": " + String(hum);
           payload += "}}";
    
    // Enviar la solicitud POST
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        Serial.printf("Datos enviados: %s\n", payload.c_str());
        Serial.printf("Respuesta del servidor: %d\n", httpResponseCode);
    } else {
        Serial.printf("Error al enviar datos: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    // Liberar recursos HTTP
    http.end();
}
