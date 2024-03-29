#ifndef DHT22_DHT22_H
#define DHT22_DHT22_H

#include <Arduino.h>

#define DEFAULT_DHT22_DETECT_SIGNAL_TIME_MS 10
#define DEFAULT_DHT22_RESPONSE_TIMEOUT_US 100
#define DEFAULT_DHT22_READ_TIMEOUT_US 100

struct DHT22Measurement {
    float humidity;
    float temperatureCelsius;
    float temperatureFahrenheit;
    bool isTemperatureNegative;
    bool isChecksumValid;
    bool isTimedOut;


};

class DHT22 {

private:

    short dht22Pin;

    int detectSignalTimeMS;
    int responseTimeoutUS;
    int readTimeoutUS;

    template<typename T, size_t S>
    long convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex);

    DHT22Measurement extractData(unsigned char (& bits)[40]);

    bool isDHT22State(const char& state);

    void sendStartSignal();

    bool waitStartSignalResponse();

    bool readData(unsigned char (& bits)[40]);

    bool isChecksumValid(unsigned char (& bits)[40]);

    bool waitState(const char& expectedState, const unsigned long& timeoutUS);

    float convertCelsiusToFahrenheit(const float& celsius);

public:

    DHT22(const short& pin);

    DHT22Measurement measure();

    DHT22Measurement measure(const int& delayMS);

    void setDetectSignalTimeMs(const int& detectSignalTimeMS);

    void setResponseTimeoutUs(const int& responseTimeoutUS);

    void setReadTimeoutUs(const int& readTimeoutUs);
};


#endif //DHT22_DHT22_H
