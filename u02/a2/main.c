#include "nnxt.h"
#include <stdio.h>
#define INTERVAL 4000
#define LEFT_MOTOR Port_B
#define RIGHT_MOTOR Port_A
#define USONIC_Sensor Port_1
#define TOUCH_LEFT Port_0
#define TOUCH_RIGHT Port_2


typedef char har;

int main(){
    //Init-Part
    SensorConfig(TOUCH_RIGHT, SensorTouch);
    uint8_t pressed = 0;
    uint16_t counter = 0;
    uint32_t start_time = 0;
    sensor_touch_clicked_t touch;
    har msg[40];
    NNXT_LCD_DisplayStringAtLine(0,"4s Intervall,");
    NNXT_LCD_DisplayStringAtLine(1,"ab ersten Klick");
    //Loop-Part
    while(1){
        Delay(50);
        Touch_Clicked(TOUCH_RIGHT, &touch);
        if(touch==SensorTouch_clicked && !pressed){
            pressed = 1;
            counter++;
            if (!start_time){
                start_time = GetSysTime();
                NNXT_LCD_DisplayStringAtLine(0,"Weiter druecken!!!");
                NNXT_LCD_DisplayStringAtLine(1,"                  ");
            }
        }
        else if(touch!=SensorTouch_clicked && pressed){
            pressed = 0;
        }

        /*
        if(start_time){
            uint32_t current_time = GetSysTime();
            current_time = counter / ((current_time - start_time) / 1000) ;
            sprintf(msg, "Frequenz %d  p/s    ", current_time);
            NNXT_LCD_DisplayStringAtLine(0,msg);
        }
        */

        if(start_time && ((GetSysTime() - start_time) > INTERVAL)){
            uint32_t freq = (counter *1000) / (INTERVAL / 1000);
            sprintf(msg, "Frequenz %d  p/s      ", freq);
            NNXT_LCD_DisplayStringAtLine(0,msg);
            Delay(10000);
        }

    }
    return 0;
}
