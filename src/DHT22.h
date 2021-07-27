#ifndef DHT22_DHT22_H
#define DHT22_DHT22_H

#include <Arduino.h>
#include <StopWatchMicros.h>

#define DHT22_TIMEOUT_MS 10

struct DHT22Measurement {
    float humidity;
    float temperature;
    bool isTemperatureNegative;
    bool isChecksumValid;
};

class DHT22 {

private:
    /*TODO: Rename to dht22Pin*/
    short pin;

    bool isValueValid(const unsigned long& value, const unsigned long& expected, const unsigned long& deviation);

    template<typename T, size_t S>
    long convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex);

    DHT22Measurement extractData(unsigned char (& bits)[40]);

    bool isDHT22State(char state);

    void sendStartSignal();

    void waitStartSignalResponse();

    void readData(unsigned char (& bits)[40]);

    char determinateBit(const unsigned long& signalLength);

    bool isChecksumValid(unsigned char (& bits)[40]);

public:

    DHT22(short pin);

    DHT22Measurement measure();
    DHT22Measurement measure(int delayMS);

};


#endif //DHT22_DHT22_H
