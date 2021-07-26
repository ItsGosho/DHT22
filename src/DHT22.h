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
    /*TODO: Rename to dht22Pin*/
    short pin;

    bool isValueValid(const unsigned long& value, const unsigned long& expected, const unsigned long& deviation);

    template<typename T, size_t S>
    long convertBinaryToDecimal(T (& binaryNumbers)[S], const long& startIndex, const long& endIndex);

    DHT22Measurement extractData(unsigned char (& bits)[40]);

    /**
     * @return If the DHT22 sensor c is providing low signal
     */
    bool isDHT22Low();

    /**
     * @return If the DHT22 sensor pin is providing high signal
     */
    bool isDHT22High();

public:

    DHT22(short pin);

    DHT22Measurement measure();

};


#endif //DHT22_DHT22_H
