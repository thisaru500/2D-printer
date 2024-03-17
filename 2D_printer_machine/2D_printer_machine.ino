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




void setup()
 Serial.begin(  9600 );
  
  penServo.attach(penServoPin);
  penServo.write(penZUp);
  delay(100);

  // Decrease if necessary
  myStepperX.setSpeed(600);

  myStepperY.setSpeed(600);  
  

  //  Set & move to initial default position
 

  //  Notifications!!!
  Serial.print("X range is"); 
  Serial.print(Xmin);  
  Serial.print(" to "); 
  Serial.print(Xmax); 
  Serial.println("  mm."); 
  Serial.print("Y range is "); 
  Serial.print(Ymin); 
  Serial.print(" to "); 
  Serial.print(Ymax); 
  Serial.println(" mm.");  
}




void  loop() 
{
  
  delay(100);
  char line[ LINE_BUFFER_LENGTH ];
  char  c;
  int lineIndex;
  bool lineIsComment, lineSemiColon;

  lineIndex  = 0;
  lineSemiColon = false;
  lineIsComment = false;

  while (1)  {

    // Serial reception - Mostly from Grbl, added semicolon support
    while ( Serial.available()>0 ) {
      c = Serial.read();
      if ((  c == '\n') || (c == '\r') ) {             // End of line reached
        if  ( lineIndex > 0 ) {                        // Line is complete. Then execute!
          line[ lineIndex ] = '\\0';                   // Terminate string
          if  (verbose) { 
            Serial.print( "Received : "); 
            Serial.println(  line ); 
          }
          processIncomingLine( line, lineIndex );
          lineIndex = 0;
        } 
        else { 
          // Empty  or comment line. Skip block.
        }
        lineIsComment = false;
        lineSemiColon = false;
        Serial.println("ok");    
      }  
      else {
        if ( (lineIsComment) || (lineSemiColon) ) {   
          if ( c == ')' )  lineIsComment = false;     
        } 
        else {
          if  ( c <= ' ' ) {                          
          } 
          else if ( c == '/' ) {                   
          }
          else if ( c == '('  ) {                    
            lineIsComment = true;
          } 
          else if  ( c == ';' ) {
            lineSemiColon = true;
          } 
          else  if ( lineIndex >= LINE_BUFFER_LENGTH-1 ) {
            Serial.println( "ERROR  - lineBuffer overflow" );
            lineIsComment = false;
            lineSemiColon  = false;
          } 
          else if ( c >= 'a' && c <= 'z' ) {        // identyfying Upcase lowercase
            line[ lineIndex++ ] = c-'a'+'A';
          }  
          else {
            line[ lineIndex++ ] = c;
          }
        }
      }
    }
  }
}

void processIncomingLine( char*  line, int charNB ) {
  int currentIndex = 0;
  char buffer[ 64 ];                             
  struct point newPos;

  newPos.x  = 0.0;
  newPos.y = 0.0;

  while( currentIndex < charNB ) {
    switch  ( line[ currentIndex++ ] ) {             
    case  'U':
      penUp(); 
      break;
    case 'D':
      penDown(); 
      break;
    case 'G':
      buffer[0] = line[ currentIndex++ ];          
    
      buffer[1] = '\\0';

      switch ( atoi( buffer ) ){                 
      case  0:                                   
      case 1:
        // /!\Dirty - Suppose that X is before Y
        char* indexX = strchr( line+currentIndex, 'X' );  
        char* indexY = strchr( line+currentIndex, 'Y' );
        if ( indexY <= 0 ) {
          newPos.x = atof( indexX + 1); 
          newPos.y  = actuatorPos.y;
        } 
        else if ( indexX <= 0 ) {
          newPos.y  = atof( indexY + 1);
          newPos.x = actuatorPos.x;
        } 
        else  {
          newPos.y = atof( indexY + 1);
          indexY = '\\0';
          newPos.x  = atof( indexX + 1);
        }
        drawLine(newPos.x, newPos.y );
        //        Serial.println("ok");
        actuatorPos.x = newPos.x;
        actuatorPos.y = newPos.y;
        break;
      }
      break;
    case 'M':
      buffer[0] = line[ currentIndex++ ]; 
      buffer[1] = line[ currentIndex++ ];
      buffer[2] = line[ currentIndex++ ];
      buffer[3] = '\\0';
      switch  ( atoi( buffer ) ){
      case 300:
        {
          char* indexS =  strchr( line+currentIndex, 'S' );
          float Spos = atof( indexS + 1);
          //         Serial.println("ok");
          if (Spos == 30) { 
            penDown(); 
          }
          if (Spos == 50) { 
            penUp();  
          }
          break;
        }
      case 114:                               
        Serial.print( "Absolute position : X = " );
        Serial.print( actuatorPos.x );
        Serial.print( "  -  Y = " );
        Serial.println( actuatorPos.y );
        break;
      default:
        Serial.print( "Command not recognized : M");
        Serial.println(  buffer );
      }
    }
  }



}
