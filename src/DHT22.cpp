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

DHT22Measurement DHT22::extractData(unsigned char (& bits)[40]) {

    float humidity = this->convertBinaryToDecimal(bits, 0, 16) / 10.0;
    float temperature = this->convertBinaryToDecimal(bits, 16, 32) / 10.0;
    float checkSum = this->convertBinaryToDecimal(bits, 32, 40);

    return DHT22Measurement{humidity, temperature};
}

//TODO: And a option to pass the delay directly here. of 2 seconds
DHT22Measurement DHT22::measure() {

    unsigned char bits[40];
    unsigned char bitIndex = 0;

    pinMode(this->pin, OUTPUT);

    digitalWrite(this->pin, LOW);
    delay(1);

    digitalWrite(this->pin, HIGH);
    pinMode(this->pin, INPUT);


    while (true) {
        bool isSensorPullLow = digitalRead(this->pin) == LOW;

        if (isSensorPullLow)
            break;
    }

    while (true) {
        bool isSensorPullHigh = digitalRead(this->pin) == HIGH;

        if (isSensorPullHigh)
            break;
    }

    int lastState = LOW;
    bool transferStarted = false;
    StopWatchMicros stopWatchMicros;
    while (true) {
        bool isLow = digitalRead(this->pin) == LOW;
        bool isHigh = digitalRead(this->pin) == HIGH;

        if (isLow) {

            if (lastState == HIGH) {

                unsigned long value = stopWatchMicros.stop();
                if (this->isValueValid(value, 80, 20)) {
                    bits[bitIndex] = 1;
                } else if (this->isValueValid(value, 27, 20)) {
                    bits[bitIndex] = 0;
                } else {
                    //Unexpected error something like that :D
                    bits[bitIndex] = 5;
                }

                bitIndex++;
            }

            lastState = LOW;
            transferStarted = true;
        }

        if (isHigh && transferStarted) {
            stopWatchMicros.run();
            lastState = HIGH;
        }

        if (bitIndex >= 40)
            break;
    }

    float humidity = this->convertBinaryToDecimal(bits, 0, 16) / 10.0;
    float temperature = this->convertBinaryToDecimal(bits, 16, 32) / 10.0;
    float checkSum = this->convertBinaryToDecimal(bits, 32, 40);

    return DHT22Measurement{humidity, temperature};
}
