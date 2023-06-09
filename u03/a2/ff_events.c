#include "nnxt.h"

typedef uint8_t EventType;
volatile uint16_t events = 0;

void setEvent(EventType ev) {
    uint16_t setter = 1 << ev;
    taskENTER_CRITICAL();
    events |= setter;
    taskEXIT_CRITICAL();
}

uint8_t eventIsSet(EventType ev) {
    uint16_t setter = 1 << ev;
    return events & setter;
}

void clearEvent(EventType ev) {
    uint16_t setter = ~(1 << ev);
    taskENTER_CRITICAL();
    events &= setter;
    taskEXIT_CRITICAL();
}
