#include "nnxt.h"
#include <stdio.h>
#define INTERVAL 5000


typedef char har;
/*
int main(){
    //Init-Part
    SensorConfig(Port_0, SensorTouch);
    uint8_t pressed = 0;
    uint16_t counter = 0;
    uint32_t start_time = 0;
    sensor_touch_clicked_t touch;
    har msg[40];
    NXT_LCD_DisplayStringAtLine(0,"5s Intervall, ab ersten Klick");
    //Loop-Part
    while(1){
        Delay(20);
        Touch_Clicked(Port_0, &touch);
        if(touch==SensorTouch_clicked && !pressed){
            pressed++;
            counter++;
            if (!start_time){
                start_time = GetSysTime();
            }
        }
        else if(touch!=SensorTouch_clicked && pressed){
            pressed = 0;
        }

        /*
        if(start_time){
            uint32_t current_time = GetSysTime();
            current_time = ((current_time - start_time) / 1000) * counter;
            sprintf(msg, "Frequenz %d  p/s    ", current_time);
            NNXT_LCD_DisplayStringAtLine(0,msg);
        }
        */
        /*
        if(start_time && ((GetSysTime() - start_time) > INTERVAL)){
            uint32_t freq = counter / (INTERVAL / 1000);
            sprintf(msg, "Frequenz %d  p/s    ", freq);
            NNXT_LCD_DisplayStringAtLine(0,msg);
            Delay(10000);
        }
    }
    return 0;
}
*/

volatile uint8_t left_pressed = 0;
volatile uint8_t right_pressed = 0;

void LeftSensorClicked(){
    while(1) {
        Delay(100);
        sensor_touch_clicked_t touch;
        Touch_Clicked(Port_0, &touch);
        left_pressed = (touch==SensorTouch_clicked) ? 1 : 0;
    }
}

void RightSensorClicked(){
    while(1) {
        Delay(100);
        sensor_touch_clicked_t touch;
        Touch_Clicked(Port_1, &touch);
        if (touch==SensorTouch_clicked){
            right_pressed ^= 1;
        }
    }
}


void Drive(){
    while(1) {
        if (left_pressed){
            motor_dir_t dir = right_pressed ? Motor_dir_backward : Motor_dir_forward;
            Motor_Drive(Port_A, dir, 30);
            Motor_Drive(Port_B, dir, 33);
            Delay(1000);
            Motor_Stop(Port_A, Motor_stop_float);
            Motor_Stop(Port_A, Motor_stop_float);
        }
    }
}

int main(){
    MotorPortInit(Port_A);
    MotorPortInit(Port_B);
    SensorConfig(Port_0, SensorTouch);
    SensorConfig(Port_1, SensorTouch);

    CreateAndStartTask(LeftSensorClicked);
    CreateAndStartTask(RightSensorClicked);
    CreateAndStartTask(Drive);

    StartScheduler();

    return 0;
}
