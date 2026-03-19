// globals.cpp
#include "globals.h"

int dacValue = 0;
int pulseWidth = 0;

bool robot_danger = false;
bool robot_warning = false;
double turnangle = 0;

volatile unsigned long throttlePulse = 0;
volatile unsigned long reversePulse = 0;
volatile unsigned long turnSwitchPulse = 0;
volatile unsigned long steeringPulse = 0;
volatile unsigned long liftPulse = 0;

double forward_speed = 0.0;



