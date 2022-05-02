#include <Stepper.h>

#define TARGET1   1
#define TARGET2   2
#define TARGET3   3
#define TARGET4   4
#define STARTCMD  "START"
#define STOPCMD   "STOP"
#define AUTOCMD   "AUTO"
#define MANUALCMD "MANUAL"

const int stepsPerRevolution = 200;

enum mode { AUTO, MANUAL, NONE };
enum automaticMode { startWait, target1, target2, target3, target4, finish};

// initialize the stepper library on pins 8 through 11:
Stepper myStepperLeft(stepsPerRevolution, 8, 9, 10, 11);
Stepper myStepperRight(stepsPerRevolution, 4, 5, 6, 7);

void setup() {
  // set the initial speed at 20 rpm:
  myStepperRight.setSpeed(20);
  myStepperLeft.setSpeed(20);
  // initialize the serial port:
  Serial.begin(9600);
  mode bigMode = NONE;
  automaticMode autoMode = startWait;
}

void loop() {

  String input;

  //State Machine (Overlord)
  switch(bigMode){
    //State Machine (Auto)
    case AUTO:

      while(bigMode == AUTO){

        switch(autoMode){
          //State 0
          case startWait:
            //Wait for start command
            //Go to next state when start command is received
            input = readPI();
            if(input == STARTCMD){
              autoMode = target1;
              input = NULL;
            }
            else if(input == STOP){
              bigMode = NONE;
            }
          
          //State 1
          case target1:
            //Drive forward approx 2 ft (554 steps per motor)
            //Wait to receive confirmation that Target 1 has been hit
            //Goes to next state upon receiving the confirmation from PI
            drive(554, 554, 60);
            input = readPI();
            if(input == STARTCMD){
              autoMode = target1;
            }
            else if(input == STOP){
              bigMode = NONE;
            }

          //State 2
          case target2:
            //Sweeping turn to be in front of Target 2
            //Wait to receive confirmation that Target 2 has been hit
            //Goes to next state upon receiving the confirmation from PI

          //State 3
          case target3:
            //Sweeping turn to be in front of Target 3
            //Wait to receive confirmation that Target 3 has been hit
            //Goes to next state upon receiving the confirmation from PI

          //State 4 (Long Shot)
          case target4:
            //Wait to receive confirmation that Target 4 has been hit
            //Goes to next state upon receiving the confirmation from PI

          //State 4 (Close Shot)
          //case target4:
            //Drive to front of 4th target
            //Wait to receive confirmation that Target 4 has been hit
            //Goes to next state upon receiving the confirmation from PI

          //State 5 (Long Shot)
          case finish:
            //Drive forward into end zone

          //State 5 (Close Shot)
          //case finish:
            //Turn 180 degrees
            //Drive forward back to area of target 3
            //Turn to face end zone
            //Drive forward into end zone
        }
      }
      break;
    case MANUAL:
    //State Machine (Manual)
      break;
    default:
      if(readPI() == AUTOCMD){
        bigMode = AUTO;
        autoMode = 
      }
      else if (readPI() == MANUALCMD){
        bigMode = MANUAL;
      }
      else{
        bigMode = NONE;
      }
    break;
  }
  /*drive(2000, 1000, 100);
  delay(2000);*/
}

void drive(int rightSteps, int leftSteps, int rpm){

  //Set the RPM's of the motors to be 
  float ratio;
  float rightRPM;
  float leftRPM;
  if(rightSteps > leftSteps){
    ratio = abs(float(rightSteps)/float(leftSteps));
    rightRPM = rpm*ratio;
    leftRPM = rpm;
  }
  else if(leftSteps > rightSteps){
    ratio = abs(float(leftSteps)/float(rightSteps));
    rightRPM = rpm;
    leftRPM = rpm*ratio;
  }
  else{
    rightRPM = rpm;
    leftRPM = rpm;
  }

  int leftCoef = leftSteps/abs(leftSteps);
  int rightCoef = rightSteps/abs(rightSteps);
  
  /*Serial.print("Right:");
  Serial.print(rightSteps);
  Serial.print(" Left:");
  Serial.print(leftSteps);
  Serial.print(" Ratio:");
  Serial.print(ratio);
  Serial.print(" Right RPM:");
  Serial.print(rightRPM);
  Serial.print(" Left RPM:");
  Serial.print(leftRPM);*/

  int rightStepsDone = 0;
  int leftStepsDone = 0;

  myStepperRight.setSpeed(rightRPM);
  myStepperLeft.setSpeed(leftRPM);
  for(int x = 0; x < abs(leftSteps) && x < abs(rightSteps); x++){
    if(rightSteps > leftSteps){
      myStepperRight.step(rightCoef*ratio);
      myStepperLeft.step(leftCoef);
      rightStepsDone += ratio;
      leftStepsDone++;
    }
    else if(leftSteps > rightSteps){
      myStepperRight.step(rightCoef);
      myStepperLeft.step(leftCoef*ratio);
      leftStepsDone += ratio;
      rightStepsDone++;
    }
    else{
      myStepperRight.step(1);
      myStepperLeft.step(1);
      rightStepsDone++;
      leftStepsDone++;
    }
  }
  /*Serial.print(" Right Steps Done:");
  Serial.print(rightStepsDone);
  Serial.print(" Left Steps Done:");
  Serial.print(leftStepsDone);
  Serial.print('\n');*/
}

String readPI(){
  if(Serial.available() > 0){
    String data = Serial.readStringUntil('\n');
    Serial.println(data);
    return data;
  }
  return NULL;
}