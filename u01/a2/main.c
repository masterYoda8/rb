#include "nnxt.h"
#define LEFT_MOTOR Port_B
#define RIGHT_MOTOR Port_A
#define USONIC_Sensor Port_1

void drive(uint32_t d){
    Motor_Drive(LEFT_MOTOR, Motor_dir_forward, 30);
    Motor_Drive(RIGHT_MOTOR, Motor_dir_forward, 33);
    Delay(d);
    Motor_Stop(LEFT_MOTOR, Motor_stop_float);
    Motor_Stop(RIGHT_MOTOR, Motor_stop_float);
    Delay(200);
}

void turnLeft(){
    Motor_Drive(LEFT_MOTOR, Motor_dir_backward, 30);
    Motor_Drive(RIGHT_MOTOR, Motor_dir_forward, 33);
    Delay(1350);
    Motor_Stop(LEFT_MOTOR, Motor_stop_break);
    Motor_Stop(RIGHT_MOTOR, Motor_stop_break);
}

int main(){
    MotorPortInit(LEFT_MOTOR);
    MotorPortInit(RIGHT_MOTOR);
    uint32_t length = 2000;
    while(1){
            /* //square
            drive(5000);
            turnLeft();
            Delay(100);
            */
            // spiral
            drive(length);
            turnLeft();
            length += 700;
    }
   	return 0;
}

