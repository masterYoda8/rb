#include "nnxt.h"
#define DEG_TO_RPS 2.7778
#define PI 3.1416
#define WHEEL_DIAMETER 4.3
#define TARGET_SPEED 9.0
#define KP 10  //P 3.5 //PD 1.5
#define KI 1.5//PI 1.5
#define KD 0.75 // PD 2.5
#define DELAY 50

uint32_t prev_deg, deg, prev_tick, dt, prev_i_time, prev_delta;
double rps, cummulativeDelta;
int8_t motorForce;

void getCurrentSpeed(double * speedInCmPerS);
double pRegulator(double diff);
double iRegulator(double diff);
double dRegulator(double diff);

int main(){
    // init
    double currentSpeed, ds;
    char msg[20];
    Motor_Tacho_GetCounter(Port_A, &prev_deg);
    prev_i_time = prev_tick = GetSysTime();
    prev_delta = prev_deg = motorForce = cummulativeDelta = 0;
    while(1){
        // calculate speed difference
        getCurrentSpeed(&currentSpeed);
        sprintf(msg, "%f    ", currentSpeed);
        NNXT_LCD_DisplayStringAtLine(0, msg);
        ds = TARGET_SPEED - currentSpeed;
        // call regulators
        motorForce = (pRegulator(ds) + iRegulator(ds) + dRegulator(ds));
        // limit motor force between 0 and 80
        motorForce = (motorForce < 0) ? 0 : (motorForce > 80) ? 80 : motorForce;
        sprintf(msg, "%d    ", motorForce);
        NNXT_LCD_DisplayStringAtLine(1, msg);
        // power motor
        Motor_Drive(Port_A, Motor_dir_forward, motorForce);
        Delay(DELAY);
    }
    return 0;
}

void getCurrentSpeed(double * speedInCmPerS){
    // get rps
    dt = GetSysTime() - prev_tick;
    Motor_Tacho_GetCounter(Port_A, &deg);
    deg = deg - prev_deg;
    rps = ((deg * 1.33) * DEG_TO_RPS) / (double) dt;
    Motor_Tacho_GetCounter(Port_A, &prev_deg);
    prev_tick = GetSysTime();
    // calculate speed
    *speedInCmPerS = PI * WHEEL_DIAMETER * rps;
}

double pRegulator(double diff){
    return KP * diff;
}

double iRegulator(double diff){
    cummulativeDelta += diff;
    int8_t ret = KI * cummulativeDelta * DELAY;
    prev_i_time = GetSysTime();
    return ret;
}

double dRegulator(double diff){
    int8_t ret = KD * (diff - prev_delta) / DELAY;
    prev_delta = diff;
    return ret;
}

