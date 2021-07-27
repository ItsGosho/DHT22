#ifndef DHT22_DHT22_H
#define DHT22_DHT22_H

#include <Arduino.h>
#include <StopWatchMicros.h>

#define DHT22_RESPONSE_TIMEOUT_US 100
#define DHT22_READ_TIMEOUT_US 100
#define DHT22_DETECT_SIGNAL_TIME_MS 10

struct DHT22Measurement {
    float humidity;
    float temperature;
    bool isTemperatureNegative;
    bool isChecksumValid;
    bool isTimedOut;
};

class DHT22 {

private:
    short dht22Pin;

    template<typename T, size_t S>
    long convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex);

    DHT22Measurement extractData(unsigned char (& bits)[40]);

    bool isDHT22State(const char& state);

    void sendStartSignal();

    bool waitStartSignalResponse();

    bool readData(unsigned char (& bits)[40]);

    bool isChecksumValid(unsigned char (& bits)[40]);

    bool waitState(const char& expectedState,const unsigned long& timeoutUS);

public:

    DHT22(const short& pin);

    DHT22Measurement measure();

    DHT22Measurement measure(int delayMS);

};


#endif //DHT22_DHT22_H
