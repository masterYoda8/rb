#include "nnxt.h"
#include "ff_events.h"
#include "ff_timer.h"
#define LEFT_MOTOR Port_B
#define RIGHT_MOTOR Port_A
#define TOUCH_LEFT Port_0
#define TOUCH_RIGHT Port_2
#define L_CLICK 0
#define R_CLICK 1
#define TIMER_ENDED 2

typedef enum {
    L1,
    L2,
    R2,
    L3,
    R3,
    L4,
    R4,
    V4,
    L5,
    R5,
    L6,
    R6,
    R7
} state;

void restartTimer(){
    clearEvent(TIMER_ENDED);
    cancelTimer(0);
    startTimer(0);
}

void turn(uint8_t steps){
    Motor_Drive(LEFT_MOTOR, Motor_dir_forward, 30);
    Motor_Drive(RIGHT_MOTOR, Motor_dir_backward, 33);
    Delay(870 * steps + 14*(steps -1));
    Motor_Stop(LEFT_MOTOR, Motor_stop_break);
    Motor_Stop(RIGHT_MOTOR, Motor_stop_break);
}

void drive(int8_t squares){
    motor_dir_t currentDirection = Motor_dir_forward;
    if (squares < 0){
        currentDirection = Motor_dir_backward;
        squares *= -1;
    }
    Motor_Drive(LEFT_MOTOR, currentDirection, 30);
    Motor_Drive(RIGHT_MOTOR, currentDirection, 33);
    Delay(500 * squares);
    Motor_Stop(LEFT_MOTOR, Motor_stop_float);
    Motor_Stop(RIGHT_MOTOR, Motor_stop_float);
    Delay(200);
}

void LeftSensorClicked(){
    uint8_t alreadyClicked = 0;
    while(1) {
        Delay(50);
        sensor_touch_clicked_t touch;
        Touch_Clicked(TOUCH_LEFT, &touch);
        if (touch==SensorTouch_clicked && !alreadyClicked){
            setEvent(L_CLICK);
            alreadyClicked = 1;
        } else if (touch!=SensorTouch_clicked && alreadyClicked) {
            alreadyClicked = 0;
        }
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


void automata(){
    state currentState = V4;
    setTimer(0, 5000, TIMER_ENDED);
    char text[10];
    while(1){
        switch (currentState){
            case L1:
                if(eventIsSet(L_CLICK)){
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = R2;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(-3);
                    turn(1);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case L2:
                if(eventIsSet(L_CLICK)){
                    drive(1);
                    currentState = L1;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = R3;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(-2);
                    turn(1);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case R2:
                if(eventIsSet(L_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = L1;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    drive(1);
                    currentState = R3;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(2);
                    turn(3);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case L3:
                if(eventIsSet(L_CLICK)){
                    drive(1);
                    currentState = L2;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = R4;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(-1);
                    turn(1);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case R3:
                if(eventIsSet(L_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = L2;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    drive(1);
                    currentState = R4;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(1);
                    turn(3);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case L4:
                if(eventIsSet(L_CLICK)){
                    drive(1);
                    currentState = L3;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = R5;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    turn(1);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case R4:
                if(eventIsSet(L_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = L3;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    drive(1);
                    currentState = R5;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    turn(3);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case V4:
                if(eventIsSet(L_CLICK)){
                    turn(3);
                    drive(1);
                    currentState = L3;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    turn(1);
                    drive(1);
                    currentState = R5;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case L5:
                if(eventIsSet(L_CLICK)){
                    drive(1);
                    currentState = L4;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = R6;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(1);
                    turn(1);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case R5:
                if(eventIsSet(L_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = L4;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    drive(1);
                    currentState = R6;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(-1);
                    turn(3);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case L6:
                if(eventIsSet(L_CLICK)){
                    drive(1);
                    currentState = L5;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = R7;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(2);
                    turn(1);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case R6:
                if(eventIsSet(L_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = L5;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    drive(1);
                    currentState = R7;
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(-2);
                    turn(3);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
            case R7:
                if(eventIsSet(L_CLICK)){
                    turn(2);
                    drive(1);
                    currentState = L6;
                    restartTimer();
                    clearEvent(L_CLICK);
                } else if (eventIsSet(R_CLICK)){
                    restartTimer();
                    clearEvent(R_CLICK);
                } else if (eventIsSet(TIMER_ENDED)){
                    drive(-3);
                    turn(3);
                    currentState = V4;
                    restartTimer();
                    clearEvent(TIMER_ENDED);
                }
                break;
        }

        sprintf(text, "%d  ", currentState);
        NNXT_LCD_DisplayStringAtLine(0, text);
    }
}
/*
void t (){

    while(1){
        if(eventIsSet(L_CLICK)){
            NNXT_LCD_DisplayStringAtLine(0, "Links ");
            clearEvent(L_CLICK);
            Delay(500);
            NNXT_LCD_ClearStringLine(0);
        } else if (eventIsSet(R_CLICK)) {
            NNXT_LCD_DisplayStringAtLine(0, "Rechts");
            clearEvent(R_CLICK);
            Delay(500);
            NNXT_LCD_ClearStringLine(0);
        }
    }
}
*/
int main(){
    MotorPortInit(LEFT_MOTOR);
    MotorPortInit(RIGHT_MOTOR);
    SensorConfig(TOUCH_LEFT, SensorTouch);
    SensorConfig(TOUCH_RIGHT, SensorTouch);


    CreateAndStartTask(timerTask);
    CreateAndStartTask(LeftSensorClicked);
    CreateAndStartTask(RightSensorClicked);

    CreateAndStartTask(automata);

    // CreateAndStartTask(t);

    StartScheduler();

    return 0;
}
