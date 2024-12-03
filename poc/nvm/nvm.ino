#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(9600);
  while (!Serial){

  }
  preferences.begin("claves", false);
  float temperatura = 0.0;
  float humedad = 0.0;
  preferences.putFloat("temperatura", temperatura);
  preferences.putFloat("humedad", humedad);
  temperatura = 10.8;
  float leerTemperatura = preferences.getFloat("temperatura", temperatura);
  float leerHumedad = preferences.getFloat("humedad", humedad);

  Serial.print("Temperatura guardada: ");
  Serial.println(leerTemperatura);
  Serial.print("Humedad guardada: ");
  Serial.println(leerHumedad);
  preferences.end();
}


void loop() {

}