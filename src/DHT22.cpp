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
        T decimalValue = binaryValue * (1 << exponent);

        convertedValue += decimalValue;
        exponent++;
    }

    return convertedValue;
}

//TODO: And a option to pass the delay directly here. of 2 seconds
DHT22Measurement DHT22::measure() {

    unsigned long counter[40];
    unsigned long counterIndexer = 0;

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
                    counter[counterIndexer] = 1;
                } else if (this->isValueValid(value, 27, 20)) {
                    counter[counterIndexer] = 0;
                } else {
                    //Unexpected error something like that :D
                    counter[counterIndexer] = 5;
                }

                counterIndexer++;
            }

            lastState = LOW;
            transferStarted = true;
        }

        if (isHigh && transferStarted) {
            stopWatchMicros.run();
            lastState = HIGH;
        }

        if (counterIndexer >= 40)
            break;
    }

    float humidity = this->convertBinaryToDecimal(counter, 0, 16) / 10.0;
    float temperature = this->convertBinaryToDecimal(counter, 16, 32) / 10.0;
    float checkSum = this->convertBinaryToDecimal(counter, 32, 40);

    return DHT22Measurement{humidity, temperature};
}
