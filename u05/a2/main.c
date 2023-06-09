#include "nnxt.h"
#include "nnxt_pcf8574lb.h"
#include "ff_events.h"
#include "ff_timer.h"

#define TOUCH_LEFT Port_2
#define TOUCH_RIGHT Port_1

#define I2C_PORT Port_3

#define LEFT_CLICK 0
#define RIGHT_CLICK 1
#define TIMER_ELAPSED 2

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


void adaptFrequency(){
    uint32_t freq = 1000;
    char msg[100];
    setTimer(0, freq, TIMER_ELAPSED);
    startTimer(0);
    while(1){
        if(eventIsSet(TIMER_ELAPSED)){
            sprintf(msg, "Freq: %d     ", freq);
            NNXT_LCD_DisplayStringAtLine(0, msg);
            DeletePort(I2C_PORT,0);
            Delay(50);
            WritePort(I2C_PORT,0);
            cancelTimer(0);
            startTimer(0);
            clearEvent(TIMER_ELAPSED);
        }
        if(eventIsSet(LEFT_CLICK)){
            freq /= 2;
            sprintf(msg, "Sub: %d     ", freq);
            NNXT_LCD_DisplayStringAtLine(0, msg);
            cancelTimer(0);
            setTimer(0, freq, TIMER_ELAPSED);
            clearEvent(TIMER_ELAPSED);
            startTimer(0);
            clearEvent(LEFT_CLICK);
        }
        if(eventIsSet(RIGHT_CLICK)){
            freq *= 2;
            sprintf(msg, "Add: %d     ", freq);
            NNXT_LCD_DisplayStringAtLine(0, msg);
            cancelTimer(0);
            setTimer(0, freq, TIMER_ELAPSED);
            clearEvent(TIMER_ELAPSED);
            startTimer(0);
            clearEvent(RIGHT_CLICK);
        }
    }
}

void test(){
    while(1){
    DeletePort(I2C_PORT, 0);
    Delay(50);
    WritePort(I2C_PORT, 0);
    Delay(1000);
    }
}

int main(){
    SensorConfig(TOUCH_LEFT, SensorTouch);
    SensorConfig(TOUCH_RIGHT, SensorTouch);


    CreateAndStartTask(timerTask);

    CreateAndStartTask(LeftSensorClicked);
    CreateAndStartTask(RightSensorClicked);

    CreateAndStartTask(adaptFrequency);

    //CreateAndStartTask(test);

    StartScheduler();

   	return 0;
}
