#ifndef DHT22_DHT22_H
#define DHT22_DHT22_H

#include <Arduino.h>
#include <StopWatchMicros.h>

struct DHT22Measurement {
    float humidity;
    float temperature;
};

class DHT22 {

private:
    short pin;

    bool isValueValid(const unsigned long& value, const unsigned long& expected, const unsigned long& deviation);

    template<typename T, size_t S>
    long convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex);

public:

    DHT22(short pin);

    DHT22Measurement measure();

};


#endif //DHT22_DHT22_H
