#ifndef FF_TIMER_H_INCLUDED
#define FF_TIMER_H_INCLUDED

typedef uint8_t EventType;
typedef uint8_t TimerType;
typedef struct {
    uint32_t interval;
    uint32_t t_end;
    uint8_t isRunning;
    EventType triggerEvent;
} timerObject;

uint8_t isRunningTimer(TimerType timer);
void setTimer(TimerType timer, uint32_t interval, EventType ev);
void startTimer(TimerType timer);
void cancelTimer(TimerType timer);
void timerTask();

#endif /* FF_TIMER_H_INCLUDED */
