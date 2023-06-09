#include "nnxt.h"
#define IR_SENSOR Port_3

uint8_t sensorCurveSize = 7;
/*
double sensorCurve[][3] = {
        {0.4, 80, 1, -150},
        {0.6 ,50, -50},
        {1, 30, -32},
        {1.25, 22, -17.5},
        {1.65, 15, -5.88},
        {2.5, 10, -15},
        {3.15, 0, 0},
};
*/

int16_t sensorCurve[][2] = {
        {1815, 30},
        {1830, 20},
        {2010, 15},
        {2355, 10},
        {3150, 5},
        {3825, 3},
        {4095, 0},
};


void calcDistance(uint16_t* distance){
    uint16_t value;
    char msg[40];
    char msg1[60];
    // Get digital sensor value
    SensorADCWithFilter(IR_SENSOR, &value);
    sprintf(msg1, "%d   ", value);
    NNXT_LCD_ClearStringLine(1);
    NNXT_LCD_DisplayStringAtLine(1, msg1);
    // find index where reference value is just below actual value
    uint8_t index;
    for(index = (sensorCurveSize -1); index >= 0; --index){
        if(sensorCurve[index][0] <= value)
            break;
    }
    //
    (index == -1) ? index++ : index;
    uint8_t indexOben = (index != 6) ? index + 1 : index;
    // a
    int16_t slope = (sensorCurve[indexOben][1] - sensorCurve[index][1]) * 1000 / (sensorCurve[indexOben][0] - sensorCurve[index][0]);

    *distance = (sensorCurve[index][1] * 1000 + slope * (value  - sensorCurve[index][0]));
}

int main(){

	uint16_t value;
	char msg[40];
	while(1)
	{
		calcDistance(&value);
		sprintf(msg, "%d", (value / 1000);
		NNXT_LCD_ClearStringLine(0);
		NNXT_LCD_DisplayStringAtLine(0 , msg);
		Delay(250);
	}
	return 0;

}

