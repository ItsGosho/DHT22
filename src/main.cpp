#include <Arduino.h>
#include <SerialPrintF.h>
#include "DHT22.h"

#define BAUD_RATE 9600
#define DHT22_PIN 10
#define MINIMAL_MEASUREMENT_DELAY_MS 2000

DHT22 dht22(DHT22_PIN);

void setup() {
    Serial.begin(BAUD_RATE);

    dht22.setDetectSignalTimeMs(10);
    dht22.setReadTimeoutUs(100);
    dht22.setResponseTimeoutUs(100);
}

void loop() {
    DHT22Measurement measurement = dht22.measure(MINIMAL_MEASUREMENT_DELAY_MS);

    serial_printf(Serial, "Humidity: %2f%%, Temperature %1f C, Temperature %1f F, Checksum: %s, Timed Out: %s\n",
                  measurement.humidity,
                  measurement.temperatureCelsius,
                  measurement.temperatureFahrenheit,
                  measurement.isChecksumValid ? "true": "false",
                  measurement.isTimedOut ? "true" : "false");

}