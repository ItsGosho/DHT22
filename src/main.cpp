#include <Arduino.h>
#include <SerialPrintF.h>
#include "DHT22.h"

#define BAUD_RATE 9600
#define DHT22_PIN 10

DHT22 dht22(DHT22_PIN);

void setup() {
    Serial.begin(BAUD_RATE);
}

void loop() {

    DHT22Measurement measurement = dht22.measure(2000);

    serial_printf(Serial, "Humidity: %2f%%, Temperature %1f, Checksum: %s, Timed Out: %s\n",
                  measurement.humidity,
                  measurement.temperature,
                  measurement.isChecksumValid ? "true": "false",
                  measurement.isTimedOut ? "true" : "false");

}