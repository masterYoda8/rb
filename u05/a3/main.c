#include "nnxt.h"
#include "nnxt_pcf8574lb.h"
#include "ff_events.h"
#include "ff_timer.h"

#define TOUCH_LEFT Port_2
#define TOUCH_RIGHT Port_1

#define I2C_Port Port_3

#define LEFT_CLICK 0
#define RIGHT_CLICK 1
#define TIMER_ELAPSED 2
#define NIGHT 3

#define HR 0
#define HY 1
#define HG 2

#define NR 3
#define NY 4
#define NG 5


typedef enum{
    HF,
    NF,
    Night
} state;

void LeftSensorClicked(){
    uint8_t alreadyClicked = 0;
    sensor_touch_clicked_t touch;
    while(1) {
        Delay(50);
        Touch_Clicked(TOUCH_LEFT, &touch);
        if (touch==SensorTouch_clicked && !alreadyClicked){
            setEvent(LEFT_CLICK);
            alreadyClicked = 1;
        } else if (touch!=SensorTouch_clicked && alreadyClicked) {
            alreadyClicked = 0;
        }
    }
}

void RightSensorClicked(){
    uint8_t alreadyClicked = 0;
    sensor_touch_clicked_t touch;
    while(1) {
        Delay(50);
        Touch_Clicked(TOUCH_RIGHT, &touch);
        if (touch==SensorTouch_clicked && !alreadyClicked){
            setEvent(RIGHT_CLICK);
            alreadyClicked = 1;
        } else if (touch!=SensorTouch_clicked && alreadyClicked) {
            alreadyClicked = 0;
        }
    }
}

void allOff(){
    WritePort(I2C_Port,HG);
    WritePort(I2C_Port,HY);
    WritePort(I2C_Port,HR);

    WritePort(I2C_Port,NR);
    WritePort(I2C_Port,NY);
    WritePort(I2C_Port,NG);
}

void init(){
    allOff();
    DeletePort(I2C_Port,HG);
    DeletePort(I2C_Port,NR);
}

void changeTraffic(state to){
    clearEvent(NIGHT);
    if (to == HF){
         // Sideroad
         WritePort(I2C_Port, NG);
         DeletePort(I2C_Port, NY);
         Delay(2000);
         WritePort(I2C_Port, NY);
         DeletePort(I2C_Port, NR);
         Delay(1000);
         // Mainroad
         DeletePort(I2C_Port, HY);
         Delay(1000);
         WritePort(I2C_Port, HR);
         WritePort(I2C_Port, HY);
         DeletePort(I2C_Port, HG);
    } else if (to == NF) {
        // Mainroad
         WritePort(I2C_Port, HG);
         DeletePort(I2C_Port, HY);
         Delay(2000);
         WritePort(I2C_Port, HY);
         DeletePort(I2C_Port, HR);
         Delay(1000);
         // Sideroad
         DeletePort(I2C_Port, NY);
         Delay(1000);
         WritePort(I2C_Port, NR);
         WritePort(I2C_Port, NY);
         DeletePort(I2C_Port, NG);
    } else {
        setEvent(NIGHT);
    }
}

void blink(){
    while(1){
        if(eventIsSet(NIGHT)){
            allOff();
            Delay(1000);
            DeletePort(I2C_Port, NY);
            Delay(1000);
        }
    }
}

void trafficLight(){
    state currentState = HF;
    setTimer(0, 10000, TIMER_ELAPSED);
    init();
    while(1){
        switch(currentState){
            case HF:
                if(eventIsSet(LEFT_CLICK)){
                    cancelTimer(0);
                    currentState = NF;
                    changeTraffic(currentState);
                    //TODO Timer
                    clearEvent(LEFT_CLICK);
                    startTimer(0);
                }
                if(eventIsSet(RIGHT_CLICK)){
                    currentState = Night;
                    changeTraffic(currentState);
                    clearEvent(RIGHT_CLICK);
                }
                break;
            case NF:
                if(eventIsSet(TIMER_ELAPSED)){
                    cancelTimer(0);
                    currentState = HF;
                    changeTraffic(currentState);
                    clearEvent(TIMER_ELAPSED);
                }
                if(eventIsSet(LEFT_CLICK)){
                    clearEvent(LEFT_CLICK);
                }
                if(eventIsSet(RIGHT_CLICK)){
                    clearEvent(RIGHT_CLICK);
                }
                break;
            case Night:
                if(eventIsSet(RIGHT_CLICK)){
                    clearEvent(NIGHT);
                    currentState = HF;
                    init();
                    clearEvent(RIGHT_CLICK);
                }
                if(eventIsSet(LEFT_CLICK)){
                    clearEvent(LEFT_CLICK);
                }
                break;
        }
    }
}



int main(){
    SensorConfig(TOUCH_LEFT, SensorTouch);
    SensorConfig(TOUCH_RIGHT, SensorTouch);

    CreateAndStartTask(timerTask);

    CreateAndStartTask(LeftSensorClicked);
    CreateAndStartTask(RightSensorClicked);

    CreateAndStartTask(blink);
    CreateAndStartTask(trafficLight);

    //CreateAndStartTask(test);

    StartScheduler();

   	return 0;
}
