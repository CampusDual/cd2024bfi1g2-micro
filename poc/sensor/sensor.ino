#include <Wire.h>
#include <Adafruit_SHTC3.h>

Adafruit_SHTC3 shtc3;

void setup() {
  Serial.begin(9600);

  Wire.begin();

  if (!shtc3.begin()) {
    Serial.println("No se pudo encontrar el sensor SHTC3");
    while (1);
  }
}

void loop() {

  sensors_event_t humidity, temperature;
  shtc3.getEvent(&humidity, &temperature);
  
  if (isnan(humidity.relative_humidity) || isnan(temperature.temperature)) {
    Serial.println("Error al leer del sensor!");
    return;
  }

  Serial.print("Humedad: ");
  Serial.print(humidity.relative_humidity);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature.temperature);
  Serial.println(" *C");

  delay(2000);
}
