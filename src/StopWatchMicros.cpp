#include "StopWatchMicros.h"

StopWatchMicros::StopWatchMicros() {
    this->start = 0;
    this->end = 0;
    this->isRunning = false;
}

void StopWatchMicros::run() {

    if (this->isRunning)
        return;

    this->start = micros();
    this->end = 0;
    this->isRunning = true;
}

bool StopWatchMicros::getIsRunning() {
    return this->isRunning;
}

unsigned long StopWatchMicros::getTime() {

    if (!this->isRunning && this->end != 0)
        return this->getTime(this->end);

    if (!this->isRunning)
        return 0;

    return this->getTime(micros());
}

unsigned long StopWatchMicros::getTime(unsigned long end) {

    if (this->start > end)
        return (this->UNSIGNED_LONG_MAX_VALUE - this->start) + end;

    return end - this->start;
}

unsigned long StopWatchMicros::stop() {

    if (!this->isRunning)
        return this->getTime(this->end);

    this->end = micros();
    this->isRunning = false;

    return this->getTime(this->end);
}

bool StopWatchMicros::reset() {

    if (this->isRunning)
        return false;

    this->start = 0;
    this->end = 0;

    return true;
}
