#include "DHT22.h"

DHT22::DHT22(short pin) : dht22Pin(pin) {

}

/**
 * Will convert given portion of the provided binary numbers to their decimal value
 *
 * For example if the binary array of 5 elements is passed with startIndex 1 and endIndex 4:
 *
 * [1, 0, 1, 1, 1]
 *
 * The values that will be taken are:
 *
 * [0, 1, 1, 1]
 *
 * The resulted decimal number:
 *
 * 7
 *
 * @param binaryNumbers The array of 0 and 1's
 * @param startIndex The start index inclusive
 * @param endIndex The index inclusive
 * @return The converted binary number to decimal
 */
template<typename T, size_t S>
long DHT22::convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex) {


    long exponent = 0;
    long convertedValue = 0;

    for (int i = endIndex; i >= startIndex; i--) {

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

    unsigned int firstOctetSum = this->convertBinaryToDecimal(bits, 0, 7);
    unsigned int secondOctetSum = this->convertBinaryToDecimal(bits, 8, 15);
    unsigned int thirdOctetSum = this->convertBinaryToDecimal(bits, 16, 23);
    unsigned int fourthOctetSum = this->convertBinaryToDecimal(bits, 24, 31);

    int checksum = this->convertBinaryToDecimal(bits, 32, 39);

    uint8_t sum = firstOctetSum + secondOctetSum + thirdOctetSum + fourthOctetSum;
    return sum == checksum;
}

/*
 * TODO: Logic for the checksum and errors
 * */
DHT22Measurement DHT22::extractData(unsigned char (& bits)[40]) {

    float humidity = this->convertBinaryToDecimal(bits, 0, 15) / 10.0;
    bool isTemperatureNegative = bits[16];
    float temperature = this->convertBinaryToDecimal(bits, 17, 31) / 10.0;
    bool isChecksumValid = this->isChecksumValid(bits);

    return DHT22Measurement{humidity, temperature, isTemperatureNegative, isChecksumValid};
}

bool DHT22::isDHT22State(char state) {
    return digitalRead(this->dht22Pin) == state;
}

void DHT22::sendStartSignal() {

    pinMode(this->dht22Pin, OUTPUT);

    digitalWrite(this->dht22Pin, LOW);
    delay(DHT22_DETECT_SIGNAL_TIME_MS);

    pinMode(this->dht22Pin, INPUT);
    delayMicroseconds(40);
}

/**
 * Will block until the given LOW/HIGH state is present on the dht22 pin.
 * If the LOW/HIGH state is present more than the timeout value, then a timeout will occur.
 *
 * @param expectedState LOW or HIGH
 * @param timeoutUS The max time that the state has to gone
 * @return If there was a timeout.
 */
bool DHT22::waitState(char expectedState, unsigned long timeoutUS) {

    unsigned long start = millis();

    while (this->isDHT22State(expectedState)) {

        if (millis() - start >= timeoutUS)
            return true;
    }

    return false;
}

/**
 * Will wait for the start responses, which are high and then low.
 *
 * @return If there was a timeout
 */
bool DHT22::waitStartSignalResponse() {

    if (this->waitState(LOW, DHT22_RESPONSE_TIMEOUT_US))
        return true;

    if (this->waitState(HIGH, DHT22_RESPONSE_TIMEOUT_US))
        return true;

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

    while (bitIndex < 40) {

        if(this->waitState(LOW, DHT22_READ_TIMEOUT_US))
            return true;

        unsigned long highLengthStart = micros();

        if(this->waitState(HIGH, DHT22_READ_TIMEOUT_US))
            return true;

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