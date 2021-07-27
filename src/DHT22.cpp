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

bool DHT22::isChecksumValid(unsigned char (& bits)[40]) {

    int firstOctetSum = this->convertBinaryToDecimal(bits, 0, 8);
    int secondOctetSum = this->convertBinaryToDecimal(bits, 8, 16);
    int thirdOctetSum = this->convertBinaryToDecimal(bits, 16, 24);
    int fourthOctetSum = this->convertBinaryToDecimal(bits, 24, 32);

    int checksum = this->convertBinaryToDecimal(bits, 32, 40);

    return firstOctetSum + secondOctetSum + thirdOctetSum + fourthOctetSum == checksum;
}

/*
 * TODO: Logic for the checksum and errors
 * */
DHT22Measurement DHT22::extractData(unsigned char (& bits)[40]) {

    float humidity = this->convertBinaryToDecimal(bits, 0, 16) / 10.0;
    bool isTemperatureNegative = bits[16];
    float temperature = this->convertBinaryToDecimal(bits, 17, 32) / 10.0;
    bool isChecksumValid = this->isChecksumValid(bits);

    return DHT22Measurement{humidity, temperature, isTemperatureNegative, isChecksumValid};
}

bool DHT22::isDHT22State(char state) {
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

    while (!this->isDHT22State(LOW)) {
    }

    while (!this->isDHT22State(HIGH)) {
    }

    while (!this->isDHT22State(LOW)) {
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

/**
 * Will loop until all of the 40 data bits are not received.
 *
 * Each loop will start recording the HIGH signal length and when the HIGH signal finishes eg: LOW signal is received
 * it will take the duration of the HIGH signal and based on the sensor's documentation will determinate if the bit is 0 or 1
 *
 * @param bits The array, which will be filled with the result data bits
 */
void DHT22::readData(unsigned char (& bits)[40]) {

    unsigned char bitIndex = 0;

    int previousSignalState = LOW;
    StopWatchMicros highSignalLengthWatch;

    while (bitIndex < 40) {

        int dht22State = digitalRead(this->pin);

        bool isBitReceived = dht22State == LOW && previousSignalState == HIGH;

        if (isBitReceived) {
            unsigned long highSignalLength = highSignalLengthWatch.stop();

            bits[bitIndex] = this->determinateBit(highSignalLength);
            bitIndex++;
        }

        if (dht22State == HIGH) {
            highSignalLengthWatch.run();
        }

        previousSignalState = dht22State;
    }
}

/*TODO: TIMEOUTS!!!*/
//TODO: And a option to pass the delay directly here. of 2 seconds
DHT22Measurement DHT22::measure() {

    this->sendStartSignal();
    this->waitStartSignalResponse();

    unsigned char bits[40];
    this->readData(bits);

    return this->extractData(bits);
}

DHT22Measurement DHT22::measure(int delayMS) {

    delay(delayMS);

    return this->measure();
}
