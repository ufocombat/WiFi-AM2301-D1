#include "DHT.h"
#define DHTPIN D7
#define LED_SCL D5
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LED_SCL, OUTPUT);   
  Serial.begin(9600);
  Serial.println("DHT21 Vers 1.01");

  dht.begin();
}

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  digitalWrite(LED_SCL, HIGH);
  delay(600);
  digitalWrite(LED_SCL, LOW);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
}

