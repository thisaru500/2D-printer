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

/* Structures, global variables    */
struct  point { 
  float x; 
  float y; 
  float z; 
};

// Current position  of plothead
struct point actuatorPos;

//  Drawing settings, should be  OK
float StepInc = 1;
int StepDelay = 0;
int LineDelay =0;
int penDelay  = 50;
 
// Calculate steps per mm. Enter here.
float  StepsPerMillimeterX = 100.0;
float StepsPerMillimeterY = 100.0;

// Drawing  robot limits, in mm
// OK to start with. Could go up to 50 mm if calibrated well.  
float Xmin = 0;
float Xmax = 40;
float Ymin = 0;
float Ymax = 40;
float  Zmin = 0;
float Zmax = 1;

float Xpos = Xmin;
float Ypos = Ymin;
float  Zpos = Zmax; 

// Set to true to get debug output.
boolean verbose = false;
