#include "DHT22.h"

DHT22::DHT22(short pin) : pin(pin) {

}

//TODO: Separate class
class StopWatchMicroseconds {

private:
    const unsigned long UNSIGNED_LONG_MAX_VALUE = 4294967295;
    unsigned long start;
    unsigned long end;
    bool isRunning;

public:

    void run() {

        if (!this->isRunning) {
            this->start = micros();
            this->isRunning = true;
        }
    }

    bool getIsRunning() const {
        return this->isRunning;
    }

    unsigned long stop() {
        this->end = micros();
        this->isRunning = false;

        if (this->start > this->end)
            return (UNSIGNED_LONG_MAX_VALUE - this->start) + this->end;

        return this->end - this->start;
    }
};

bool isToleranced(unsigned long value, unsigned long expected, unsigned long tolerance) {
    return value == expected ||
           (value >= expected - tolerance && value <= expected + tolerance);
}

template<typename T, size_t S>
long convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex) {

    long exponent = 0;
    long convertedValue = 0;

    for (int i = endIndex - 1; i >= startIndex; i--) {
        T value = binaryNumbers[i];
        convertedValue += value * (1 << exponent);
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
    StopWatchMicroseconds stopWatchMicroseconds;
    while (true) {
        bool isLow = digitalRead(this->pin) == LOW;
        bool isHigh = digitalRead(this->pin) == HIGH;

        if (isLow) {

            if (lastState == HIGH) {

                unsigned long value = stopWatchMicroseconds.stop();
                if (isToleranced(value, 80, 20)) {
                    counter[counterIndexer] = 1;
                } else if (isToleranced(value, 27, 20)) {
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
            stopWatchMicroseconds.run();
            lastState = HIGH;
        }

        if (counterIndexer >= 40)
            break;
    }

    float humidity = convertBinaryToDecimal(counter, 0, 16) / 10.0;
    float temperature = convertBinaryToDecimal(counter, 16, 32) / 10.0;
    float checkSum = convertBinaryToDecimal(counter, 32, 40);

    return DHT22Measurement{humidity, temperature};
}
