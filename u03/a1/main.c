#include "nnxt.h"
#include "stdbool.h"

typedef uint8_t EventType;
volatile uint16_t events;

typedef struct {
    uint32_t interval;
    EventType event;
    uint32_t timer_end;
} TimerType;
volatile TimerType timer;



void setEvent(EventType ev) {
    uint16_t setter = 1 << ev;
    events |= setter;
}

bool eventIsSet(EventType ev) {
    uint16_t setter = 1 << ev;
    return (bool) events & setter;
}

void clearEvent(EventType ev) {
    uint16_t setter = ~(1 << ev);
    events &= setter;
}


void setTimer(TimerType *timer, uint32_t time, EventType ev){
    timer->interval = time;
    timer->event = ev;
    timer->timer_end = NULL;
}

void startTimer(TimerType *timer){
    timer->timer_end = GetSysTime() + timer->interval;
    while(1){
        if (timer->timer_end != NULL && (GetSysTime() >= timer->timer_end)){
            setEvent(timer->event);
            timer->timer_end = NULL;
            break;
        }
    }
}

void cancelTimer(TimerType *timer){
    timer->timer_end = NULL;
}


