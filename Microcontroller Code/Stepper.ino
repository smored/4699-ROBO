#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepperLeft(stepsPerRevolution, 8, 9, 10, 11);
Stepper myStepperRight(stepsPerRevolution, 4, 5, 6, 7);

void setup() {
  // set the speed at 20 rpm:
  myStepperRight.setSpeed(20);
  myStepperLeft.setSpeed(20);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  drive(2000, 1000, 100);
  delay(2000);
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
  
  Serial.print("Right:");
  Serial.print(rightSteps);
  Serial.print(" Left:");
  Serial.print(leftSteps);
  Serial.print(" Ratio:");
  Serial.print(ratio);
  Serial.print(" Right RPM:");
  Serial.print(rightRPM);
  Serial.print(" Left RPM:");
  Serial.print(leftRPM);

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
  Serial.print(" Right Steps Done:");
  Serial.print(rightStepsDone);
  Serial.print(" Left Steps Done:");
  Serial.print(leftStepsDone);
  Serial.print('\n');
}
