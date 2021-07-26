#include "DHT22.h"

DHT22::DHT22(short pin) : pin(pin) {

}

bool DHT22::isValueValid(const unsigned long& value, const unsigned long& expected, const unsigned long& deviation) {

    unsigned long lowestPoint = expected - deviation;
    unsigned long upperPoint = expected + deviation;

    return value >= lowestPoint && value <= upperPoint;
}

template<typename T, size_t S>
long DHT22::convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex) {


    long exponent = 0;
    long convertedValue = 0;

    for (int i = endIndex - 1; i >= startIndex; i--) {

        T binaryValue = binaryNumbers[i];
        long decimalValue = binaryValue * (1 << exponent);

        convertedValue += decimalValue;
        exponent++;
    }

    return convertedValue;
}

/*
 * TODO: Logc for the checksum and errors
 * */
DHT22Measurement DHT22::extractData(unsigned char (& bits)[40]) {

    float humidity = this->convertBinaryToDecimal(bits, 0, 16) / 10.0;
    float temperature = this->convertBinaryToDecimal(bits, 16, 32) / 10.0;
    float checkSum = this->convertBinaryToDecimal(bits, 32, 40);

    return DHT22Measurement{humidity, temperature};
}

bool DHT22::isDHT22(char state) {
    return digitalRead(this->pin) == state;
}

void DHT22::sendStartSignal() {

    pinMode(this->pin, OUTPUT);

    digitalWrite(this->pin, LOW);
    delay(1);

    digitalWrite(this->pin, HIGH);
    pinMode(this->pin, INPUT);
}

void DHT22::waitStartSignalResponse() {

    while (!this->isDHT22(LOW)) {
    }

    while (!this->isDHT22(HIGH)) {
    }

    while (!this->isDHT22(LOW)) {
    }
}

char DHT22::determinateBit(const unsigned long& signalLength) {

    if (this->isValueValid(signalLength, 80, 20)) {
        return 1;
    }

    if (this->isValueValid(signalLength, 27, 20)) {
        return 0;
    }

    return -1;
}

void DHT22::readData(unsigned char (& bits)[40]) {
    unsigned char bitIndex = 0;

    int lastSignalState = LOW;
    StopWatchMicros stopWatchMicros;

    while (bitIndex < 40) {

        int signalState = digitalRead(this->pin);

        if (signalState == LOW) {

            if (lastSignalState == HIGH) {

                unsigned long highSignalLength = stopWatchMicros.stop();

                bits[bitIndex] = this->determinateBit(highSignalLength);

                bitIndex++;
            }
        }

        if (signalState == HIGH) {
            stopWatchMicros.run();
        }

        lastSignalState = signalState;
    }
}

//TODO: And a option to pass the delay directly here. of 2 seconds
DHT22Measurement DHT22::measure() {

    this->sendStartSignal();
    this->waitStartSignalResponse();

    unsigned char bits[40];
    this->readData(bits);

    return this->extractData(bits);
}
