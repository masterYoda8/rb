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
}

void turnLeft(){
    Motor_Drive(LEFT_MOTOR, Motor_dir_backward, 30);
    Motor_Drive(RIGHT_MOTOR, Motor_dir_forward, 33);
    Delay(100);
    Motor_Stop(LEFT_MOTOR, Motor_stop_float);
    Motor_Stop(RIGHT_MOTOR, Motor_stop_float);
}

uint8_t detektiereObjekt(){
    sensor_error_t err;
    uint8_t dist;
    char distStr[40];
    err = US_GetDistance(USONIC_Sensor, &dist);
    if (err==sensor_error_SensorNotConfigured){
        NNXT_LCD_DisplayStringAtLine(0, "Sensor Error!!!!   ");
    } else {
        sprintf(distStr, "Entfernung: %d          ", dist);
        NNXT_LCD_DisplayStringAtLine(0, distStr);
    }
    return dist;
}

int main(){
    //Init Code
    SensorConfig(USONIC_Sensor, SensorUS);
    MotorPortInit(LEFT_MOTOR);
    MotorPortInit(RIGHT_MOTOR);
    uint8_t dist;
    uint8_t counter = 0;
    //Loop
    while(1){
        dist = detektiereObjekt();
        if (dist <= 30){
            //Turns left if 5 values smaller than 30 detected
            switch (counter) {
                case 5:
                    turnLeft();
                    counter = 0;
                    NNXT_LCD_DisplayStringAtLine(0, "Obstacle detected!!!   ");
                    break;
                default:
                    counter++;
            }
        }
        else{
            drive(200);
        }
    }
   	return 0;
}

