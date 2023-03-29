#include "nnxt.h"
#include <stdio.h>

void forward(uint16_t delay);
void turnLeft();
void square();
void spiral(uint16_t s);

int main(){
    MotorPortInit(Port_A);
    MotorPortInit(Port_B);
    while(1){
        spiral(500);
    }
    return 0;
}

void forward(uint16_t d){
    Motor_Drive(Port_A, Motor_dir_forward, 50);
    Motor_Drive(Port_B, Motor_dir_forward, 50);
    Delay(d);
    Motor_Stop(Port_A, Motor_stop_float);
    Motor_Stop(Port_B, Motor_stop_float);
    Delay(500);
}

void turnLeft(){
    Motor_Drive(Port_A, Motor_dir_backward, 30);
    Motor_Drive(Port_B, Motor_dir_forward, 30);
    Delay(1200);
    Motor_Stop(Port_A, Motor_stop_break);
    Motor_Stop(Port_B, Motor_stop_break);
    Delay(200);
}

void square(){
    for(uint8_t i = 0; i < 4; i++){
        forward(3000);
        turnLeft();
    }
}

void spiral(uint16_t s){
    for(uint8_t i = 0; i < 10; i++){
        forward(s + i * 500);
        turnLeft();
    }
}
