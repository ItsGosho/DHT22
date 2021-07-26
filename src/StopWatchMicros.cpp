#include "StopWatchMicros.h"

void StopWatchMicros::run() {

    if (this->isRunning)
        return;

    this->start = micros();
    this->isRunning = true;
}

bool StopWatchMicros::getIsRunning() {
    return this->isRunning;
}

unsigned long StopWatchMicros::stop() {
    this->end = micros();
    this->isRunning = false;

    if (this->start > this->end)
        return (this->UNSIGNED_LONG_MAX_VALUE - this->start) + this->end;

    return this->end - this->start;
}