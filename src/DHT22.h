#ifndef DHT22_DHT22_H
#define DHT22_DHT22_H

#include <Arduino.h>

struct DHT22Measurement {
    float humidity;
    float temperature;
};

class DHT22 {

private:
    short pin;

public:

    DHT22(short pin);

    DHT22Measurement measure();

};


#endif //DHT22_DHT22_H
