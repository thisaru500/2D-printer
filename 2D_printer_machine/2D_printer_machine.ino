#include  <Servo.h>
#include <AFMotor.h>

#define LINE_BUFFER_LENGTH 512

char  STEP = MICROSTEP ;

// Servo position for Up and Down 
const int penZUp  = 115;
const int penZDown = 83;

// Servo on PWM pin 10
const int penServoPin  =10 ;// only pwm pin use to do this wok

// Should be right for DVD steppers, but is not too important here
const  int stepsPerRevolution = 48; 

// create servo object to control a servo 
Servo  penServo;  

// Identifying X- and Y-axis
AF_Stepper myStepperY(stepsPerRevolution,1);            
AF_Stepper  myStepperX(stepsPerRevolution,2);
