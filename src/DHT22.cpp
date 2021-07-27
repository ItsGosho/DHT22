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

/**
 * Will compare the sum of the first 4 octet's last 8 bits with the last 1 octet
 *
 * I have posted a question/answer to that, because the explanation in the documentation can be tricky
 * https://stackoverflow.com/questions/68547020/dht22-sensors-checksum-not-valid
 *
 * @param bits The data bits
 * @return If the checksum is valid
 */
bool DHT22::isChecksumValid(unsigned char (& bits)[40]) {

    unsigned int firstOctetSum = this->convertBinaryToDecimal(bits, 0, 8);
    unsigned int secondOctetSum = this->convertBinaryToDecimal(bits, 8, 16);
    unsigned int thirdOctetSum = this->convertBinaryToDecimal(bits, 16, 24);
    unsigned int fourthOctetSum = this->convertBinaryToDecimal(bits, 24, 32);

    int checksum = this->convertBinaryToDecimal(bits, 32, 40);

    uint8_t sum = firstOctetSum + secondOctetSum + thirdOctetSum + fourthOctetSum;
    return sum == checksum;
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
    delay(18);

    pinMode(this->pin, INPUT);
    delayMicroseconds(40);
}

bool DHT22::waitStartSignalResponse() {

    unsigned long start = millis();

    while (this->isDHT22State(LOW)) {

        if (millis() - start >= DHT22_RESPONSE_TIMEOUT_MS)
            return true;
    }

    unsigned long start2 = millis();

    while (this->isDHT22State(HIGH)) {

        if (millis() - start2 >= DHT22_RESPONSE_TIMEOUT_MS)
            return true;

    }


    return false;
}

/**
 * Will loop until all of the 40 data bits are not received.
 *
 * Each loop will wait until the low signal is passed and time the high signal.
 * The length of the high signal determinants the bit 0 or 1
 *
 * @param bits The array, which will be filled with the result data bits
 * @return If the reading has timed out
 */
bool DHT22::readData(unsigned char (& bits)[40]) {

    unsigned char bitIndex = 0;

    for (uint8_t i = 40; i != 0; i--) {

        unsigned lowLengthStart = micros();

        while (digitalRead(this->pin) == LOW) {
            if (micros() - lowLengthStart >= DHT22_RESPONSE_TIMEOUT_MS)
                return true;
        }

        unsigned long highLengthStart = micros();

        while (digitalRead(this->pin) == HIGH) {

            if (micros() - highLengthStart >= DHT22_RESPONSE_TIMEOUT_MS)
                return true;
        }

        unsigned long highLengthEnd = micros();
        unsigned long highSignalLength = highLengthEnd - highLengthStart;

        bits[bitIndex] = highSignalLength > 45;
        bitIndex++;
    }

    return false;
}


DHT22Measurement DHT22::measure() {

    this->sendStartSignal();
    bool isStartResponseTimedOut = this->waitStartSignalResponse();

    if (isStartResponseTimedOut) {
        return DHT22Measurement{0, 0, false, false, true};
    }

    unsigned char bits[40];
    bool isReadingDataTimedOut = this->readData(bits);

    if (isReadingDataTimedOut) {
        return DHT22Measurement{0, 0, false, false, true};
    }

    return this->extractData(bits);
}

DHT22Measurement DHT22::measure(int delayMS) {

    delay(delayMS);

    return this->measure();
}