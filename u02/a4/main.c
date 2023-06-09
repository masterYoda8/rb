#include "nnxt.h"
#define LEFT_MOTOR Port_B
#define RIGHT_MOTOR Port_A
#define TOUCH_LEFT Port_0
#define TOUCH_RIGHT Port_2


volatile uint8_t left_pressed = 0;
volatile uint8_t right_pressed = 0;

void LeftSensorClicked(){
    while(1) {
        Delay(100);
        sensor_touch_clicked_t touch;
        Touch_Clicked(TOUCH_LEFT, &touch);
        if (touch==SensorTouch_clicked) {left_pressed = 1;}
    }
}

void RightSensorClicked(){
    uint8_t recently_right_pressed = 0;
    while(1) {
        Delay(100);
        sensor_touch_clicked_t touch;
        Touch_Clicked(TOUCH_RIGHT, &touch);
        if (touch==SensorTouch_clicked && recently_right_pressed == 0){
            right_pressed = 1;
            recently_right_pressed = 1;
        } else if (touch!=SensorTouch_clicked && recently_right_pressed == 1) {
            recently_right_pressed = 0;
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

        if (right_pressed) {
            current_direction = (current_direction == Motor_dir_forward) ? Motor_dir_backward : Motor_dir_forward;
            right_pressed = 0;
        }

        if (left_pressed){
            Motor_Drive(LEFT_MOTOR, current_direction, 33);
            Motor_Drive(RIGHT_MOTOR, current_direction, 30);
            Delay(1000);
            Motor_Stop(LEFT_MOTOR, Motor_stop_float);
            Motor_Stop(RIGHT_MOTOR, Motor_stop_float);
            left_pressed = 0;
        }

    }
}

int main(){
    MotorPortInit(LEFT_MOTOR);
    MotorPortInit(RIGHT_MOTOR);
    SensorConfig(TOUCH_LEFT, SensorTouch);
    SensorConfig(TOUCH_RIGHT, SensorTouch);

    CreateAndStartTask(LeftSensorClicked);
    CreateAndStartTask(RightSensorClicked);
    CreateAndStartTask(Drive);

    StartScheduler();

    return 0;
}
