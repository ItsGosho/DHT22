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
}

//TODO: And a option to pass the delay directly here. of 2 seconds
DHT22Measurement DHT22::measure() {

    this->sendStartSignal();
    this->waitStartSignalResponse();

    unsigned char bits[40];
    unsigned char bitIndex = 0;

    int lastState = LOW;
    bool transferStarted = false;
    StopWatchMicros stopWatchMicros;

    while (true) {

        if (this->isDHT22(LOW)) {

            if (lastState == HIGH) {

                unsigned long highSignalLength = stopWatchMicros.stop();

                if (this->isValueValid(highSignalLength, 80, 20)) {
                    bits[bitIndex] = 1;
                }

                if (this->isValueValid(highSignalLength, 27, 20)) {
                    bits[bitIndex] = 0;
                }

                bitIndex++;
            }

            lastState = LOW;
            transferStarted = true;
        }

        if (this->isDHT22(HIGH) && transferStarted) {
            stopWatchMicros.run();
            lastState = HIGH;
        }

        if (bitIndex >= 40)
            break;
    }

    return this->extractData(bits);
}
