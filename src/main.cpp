#include <Arduino.h>

bool oneTime = false;

#define DHT22_PIN 10

void setup() {
    Serial.begin(9600);
}

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

void loop() {


    unsigned long counter[40];
    unsigned long counterIndexer = 0;
    if (!oneTime) {
        //delay(2000);

        pinMode(DHT22_PIN, OUTPUT);

        digitalWrite(DHT22_PIN, LOW);
        delay(1);

        digitalWrite(DHT22_PIN, HIGH);
        pinMode(DHT22_PIN, INPUT);


        while (true) {
            bool isSensorPullLow = digitalRead(DHT22_PIN) == LOW;

            if (isSensorPullLow)
                break;
        }

        while (true) {
            bool isSensorPullHigh = digitalRead(DHT22_PIN) == HIGH;

            if (isSensorPullHigh)
                break;
        }

        int lastState = LOW;
        bool transferStarted = false;
        StopWatchMicroseconds stopWatchMicroseconds;
        while (true) {
            bool isLow = digitalRead(DHT22_PIN) == LOW;
            bool isHigh = digitalRead(DHT22_PIN) == HIGH;

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

        for (int i = 0; i < 40; ++i) {
            Serial.print(counter[i]);
            Serial.print(",");
        }

        Serial.println();

        Serial.println("Humidity:");
        for (int i = 0; i < 16; i++) {
            Serial.print(counter[i]);
        }


        float temperature = convertBinaryToDecimal(counter, 16, 32) / 10.0;

        Serial.println("");
        Serial.println("Temp:");
        Serial.println(temperature);
        Serial.println("");

        for (int i = 16; i < 32; ++i) {
            Serial.print(counter[i]);
        }

        Serial.println("");
        Serial.println("Checksum:");
        for (int i = 32; i < 40; i++) {
            Serial.print(counter[i]);
        }

        oneTime = true;
    }
}

/*
 *
 * while (true) {
            bool isSensorHigh = digitalRead(DHT22_PIN) == HIGH;
            bool isSensorLow = digitalRead(DHT22_PIN) == LOW;

            if (isSensorLow && !isSensorLowStarted) {
                isSensorLowStarted = true;
            }

            if (isSensorLowStarted && isSensorHigh) {
                microsecondsHigh++;
                delayMicroseconds(1);
            }

            if (isSensorLow && isSensorHighStarted) {
                dataBits[dataBitsIndexer] = microsecondsHigh <= 28 ? 0 : 1;
                dataBitsIndexer++;
                bool isSensorLowStarted = false;
                bool isSensorHighStarted = false;
                int microsecondsHigh = 0;
            }

            if (dataBitsIndexer == 39)
                break;
        }
 *
 * */

/*
 *  //test

        //Low response
        elements[0] = digitalRead(DHT22_PIN);
        delayMicroseconds(DHT22_PIN);

        //High response
        elements[1] = digitalRead(DHT22_PIN);
        delayMicroseconds(80);

        //bits

        int indexCounter = 0;
        bool compensate = false;
        for (int i = 0; i < 40; i++) {
            int firstDelay = 50;

            if (compensate)
                firstDelay = firstDelay - 1;

            delayMicroseconds(firstDelay);

            delayMicroseconds(29);
            bool isLow = digitalRead(DHT22_PIN) == LOW;

            if (isLow) {
                elements[indexCounter] = 0;
                compensate = true;
            } else {
                elements[indexCounter] = 1;
                delayMicroseconds(41);
                compensate = false;
            }

            indexCounter++;
        }

        oneTime = true;

        for (int i = 0; i < 40; ++i) {
            Serial.println(elements[i]);
        }
 *
 *
 * */