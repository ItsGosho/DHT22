#ifndef DHT22_STOPWATCHMICROS_H
#define DHT22_STOPWATCHMICROS_H

#include "Arduino.h"

class StopWatchMicros {

private:
    const unsigned long UNSIGNED_LONG_MAX_VALUE = 4294967295;
    unsigned long start;
    unsigned long end;
    bool isRunning;

    unsigned long getTime(unsigned long end);

public:
    StopWatchMicros();

public:
    void run();

    bool getIsRunning();

    unsigned long getTime();

    unsigned long stop();

    bool reset();
};


#endif //DHT22_STOPWATCHMICROS_H
