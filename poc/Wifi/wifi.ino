#include <WiFi.h>
const char* ssid = "A71A";
const char* password = "x2zthDAzSEQh5h";


void setup() {
  Serial.begin(115200); 
  WiFi.begin(ssid, password);
  
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}


void loop() {


}