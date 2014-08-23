// Modification of Fabscan base scanner for both ramp1.4 and advanced funcations
// Ceb Baker 2/15/2014
// Vertical finction and two lasers added


#define LIGHT_PIN 9
#define LASER_PIN 8
#define MS_PIN    14
#define LIGHT_SWITCH 3
int LIGHTState = 0;         // variable for reading the pushbutton status
//Stepper 1 as labeled on Shield, Turntable
#define ENABLE_PIN_0  38
#define STEP_PIN_0    54
#define DIR_PIN_0     55

//Stepper 2, Laser Vertical Stepper
#define ENABLE_PIN_1  56
#define STEP_PIN_1    60
#define DIR_PIN_1     61

//Stepper 3, currently unused
#define ENABLE_PIN_2  11
#define STEP_PIN_2    12
#define DIR_PIN_2     13
  
#define TURN_LASER_OFF      200
#define TURN_LASER_ON       201
#define PERFORM_STEP        202
#define SET_DIRECTION_CW    203
#define SET_DIRECTION_CCW   204
#define TURN_STEPPER_ON     205
#define TURN_STEPPER_OFF    206
#define TURN_LIGHT_ON       207
#define TURN_LIGHT_OFF      208
#define ROTATE_VERTICAL     209
#define FABSCAN_PING        210
#define FABSCAN_PONG        211
#define SELECT_STEPPER      212
#define VERTICAL_STEPPER    12
#define TURNTABLE_STEPPER   10
//the protocol: we send one byte to define the action what to do.
//If the action is unary (like turnung off the light) we only need one byte so we are fine.
//If we want to tell the stepper to turn, a second byte is used to specify the number of steps.
//These second bytes are defined here below.

#define ACTION_BYTE         1    //normal byte, first of new action
#define LIGHT_INTENSITY     2
#define STEPPER_STEPS       3
#define STEPPER_ID          7

int incomingByte = 0;
int byteType = 1;
int currStepper;
uint16_t i;

//turntable step
void step(){
{
  // initialize the LED pin as an output:
  pinMode(LIGHT_PIN, OUTPUT);      
  // initialize the pushbutton pin as an input:
  pinMode(LIGHT_SWITCH, INPUT);     
}
{
 if(currStepper == TURNTABLE_STEPPER){
   digitalWrite(STEP_PIN_0, 0);
 }else if(currStepper == VERTICAL_STEPPER){
   digitalWrite(STEP_PIN_1, 0);
 }

 delay(3);
 if(currStepper == TURNTABLE_STEPPER){
   digitalWrite(STEP_PIN_0, 1);
 }else if(currStepper == VERTICAL_STEPPER){
   digitalWrite(STEP_PIN_1, 1);
 }
 delay(3);
}}

void step(int count)
{
  for(int i=0; i<count; i++){
    step();
  }
}

void setup() 
{ 
  // initialize the serial port
   Serial.begin(9600);
   pinMode(LASER_PIN, OUTPUT);
   pinMode(LIGHT_PIN, OUTPUT);
 
   pinMode(MS_PIN, OUTPUT);
   digitalWrite(MS_PIN, HIGH);  //HIGH for 16microstepping, LOW for no microstepping

  pinMode(ENABLE_PIN_0, OUTPUT);
  pinMode(DIR_PIN_0, OUTPUT);
  pinMode(STEP_PIN_0, OUTPUT);

  pinMode(ENABLE_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
 
  pinMode(ENABLE_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
 
 //enable turntable and laser steppers
 digitalWrite(ENABLE_PIN_0, HIGH);  //LOW to turn
 digitalWrite(ENABLE_PIN_1, HIGH);  //LOW to turn
 digitalWrite(ENABLE_PIN_2, LOW);  //LOW to turn off
 
 digitalWrite(LIGHT_PIN, 0); //turn light off

 digitalWrite(LASER_PIN, 1); //turn laser on
 Serial.write(FABSCAN_PONG); //send a pong back to the computer so we know setup is done and that we are actually dealing with a FabScan
 
 currStepper = TURNTABLE_STEPPER;  //turntable is default stepper
} 

void loop() 
{
  {
  // read the state of the pushbutton value:
  LIGHTState = digitalRead(LIGHT_SWITCH);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (LIGHTState == HIGH) {     
    // turn LED on:    
    digitalWrite(LIGHT_PIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LIGHT_PIN, LOW); 
  }
}

  if(Serial.available() > 0){

    incomingByte = Serial.read();
    
    switch(byteType){
      case ACTION_BYTE:
      
          switch(incomingByte){    //this switch always handles the first byte
            //Laser
            case TURN_LASER_OFF:
              digitalWrite(LASER_PIN, LOW);    // turn the LASER off
              break;
            case TURN_LASER_ON:
              digitalWrite(LASER_PIN, HIGH);   // turn the LASER on
              break;
            //TurnTable
            case PERFORM_STEP:
              byteType = STEPPER_STEPS;
              break;
            case SET_DIRECTION_CW:
              if(currStepper == TURNTABLE_STEPPER){
                digitalWrite(DIR_PIN_0, 1);
              }else if(currStepper == VERTICAL_STEPPER){
                digitalWrite(DIR_PIN_1, 1);
              }
              break;
            case SET_DIRECTION_CCW:
              if(currStepper == TURNTABLE_STEPPER){
                digitalWrite(DIR_PIN_0, 0);
              }else if(currStepper == VERTICAL_STEPPER){
                digitalWrite(DIR_PIN_1, 0);
              }
              break;
            case TURN_STEPPER_ON:
              if(currStepper == TURNTABLE_STEPPER){
                digitalWrite(ENABLE_PIN_0, 0);
              }else if(currStepper == VERTICAL_STEPPER){
                digitalWrite(ENABLE_PIN_1, 0);
              }
              break;
            case TURN_STEPPER_OFF:
              if(currStepper == TURNTABLE_STEPPER){
                digitalWrite(ENABLE_PIN_0, 1);
              }else if(currStepper == VERTICAL_STEPPER){
                digitalWrite(ENABLE_PIN_1, 1);
              }
              break;
            case TURN_LIGHT_ON:
              byteType = LIGHT_INTENSITY;
              break;
            case TURN_LIGHT_OFF:
              digitalWrite(LIGHT_PIN, 0);
              break;
            case FABSCAN_PING:
              delay(1);
              Serial.write(FABSCAN_PONG);
              break;
            case SELECT_STEPPER:
              byteType = STEPPER_ID;
              break;
            }
      
          break;
       case LIGHT_INTENSITY:       //after this point we take care of the second byte if one is sent
          analogWrite(LIGHT_PIN, incomingByte);
          byteType = ACTION_BYTE;  //reset byteType
          break;
        case STEPPER_STEPS:
          if(currStepper == TURNTABLE_STEPPER){
                step(incomingByte);
          }else if(currStepper == VERTICAL_STEPPER){
                moveNumberOfMM(incomingByte, currStepper);
          }
          byteType = ACTION_BYTE;
          break;
        case STEPPER_ID:
          Serial.write(incomingByte);
          currStepper = incomingByte;
          byteType = ACTION_BYTE;
          break;
    }
  } 
} 

//Move Number of mm for stepper
void moveNumberOfMM(int numberMM, int stepper)
{
  float numberOfStepsPerMM;
  numberOfStepsPerMM = numberMM * 100;
  
  for (i=1; i<numberOfStepsPerMM; i++) {
     oneStep(stepper);
   }  
}

// Move one step
void oneStep(int stepper) {
  digitalWrite(stepper, HIGH);
    delayMicroseconds(5);
  digitalWrite(stepper, LOW);
}








