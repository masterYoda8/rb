#include "nnxt.h"
#include "stdbool.h"
#include "ff_events.h"
#include "ff_timer.h"
#define LEFT_MOTOR Port_B
#define RIGHT_MOTOR Port_A
#define TOUCH_LEFT Port_0
#define TOUCH_RIGHT Port_2
#define L_CLICK 0
#define R_CLICK 1
#define TIMER_FINISHED 2

void LeftSensorClicked(){
    while(1) {
        Delay(50);
        sensor_touch_clicked_t touch;
        Touch_Clicked(TOUCH_LEFT, &touch);
        if (touch==SensorTouch_clicked) {setEvent(L_CLICK);}
    }
}

void RightSensorClicked(){
    uint8_t alreadyClicked = 0;
    while(1) {
        Delay(50);
        sensor_touch_clicked_t touch;
        Touch_Clicked(TOUCH_RIGHT, &touch);
        if (touch==SensorTouch_clicked && !alreadyClicked){
            setEvent(R_CLICK);
            alreadyClicked = 1;
        } else if (touch!=SensorTouch_clicked && alreadyClicked) {
            alreadyClicked = 0;
        }
    }
}


void Drive(){
    motor_dir_t current_direction = Motor_dir_forward;
    while(1) {
        if (current_direction==Motor_dir_backward) {
                NNXT_LCD_DisplayStringAtLine(0, "hinten  ");
        } else {
                NNXT_LCD_DisplayStringAtLine(0, "vorne ");
        }

        if (eventIsSet(R_CLICK)) {
            current_direction = (current_direction == Motor_dir_forward) ? Motor_dir_backward : Motor_dir_forward;
            clearEvent(R_CLICK);
        }

        if (eventIsSet(L_CLICK)){
            Motor_Drive(LEFT_MOTOR, current_direction, 33);
            Motor_Drive(RIGHT_MOTOR, current_direction, 30);
            clearEvent(L_CLICK);
            setTimer(0, 1000, TIMER_FINISHED);
            startTimer(0);
        }

        if (eventIsSet(TIMER_FINISHED)){
            Motor_Stop(LEFT_MOTOR, Motor_stop_float);
            Motor_Stop(RIGHT_MOTOR, Motor_stop_float);
            clearEvent(TIMER_FINISHED);
        }

    }
}

int main(){
    MotorPortInit(LEFT_MOTOR);
    MotorPortInit(RIGHT_MOTOR);
    SensorConfig(TOUCH_LEFT, SensorTouch);
    SensorConfig(TOUCH_RIGHT, SensorTouch);

    CreateAndStartTask(timerTask);
    CreateAndStartTask(LeftSensorClicked);
    CreateAndStartTask(RightSensorClicked);
    CreateAndStartTask(Drive);

    StartScheduler();

    return 0;
}


