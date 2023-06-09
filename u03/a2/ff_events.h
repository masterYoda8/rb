#ifndef FF_EVENTS_H_INCLUDED
#define FF_EVENTS_H_INCLUDED

typedef uint8_t EventType;

void setEvent(EventType ev);
uint8_t eventIsSet(EventType ev);
void clearEvent(EventType ev);

#endif /* FF_EVENTS_H_INCLUDED */
