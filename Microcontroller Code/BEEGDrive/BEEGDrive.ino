#include <AccelStepper.h>
#include "BEEGDrive.h"

enum mode { AUTO, MANUAL, NONE };
enum automaticMode { startWait, target1, target2, target3, target4, finishAUTO };
enum movementState { forward, turn, forward_second, turn_complete };
String input = "NULL";

mode bigMode;
automaticMode autoMode;
movementState target2State;
movementState target3State;

// initialize the stepper library on pins 8 through 11:
AccelStepper leftMotor(AccelStepper::FULL4WIRE, IN1_Left, IN2_Left, IN3_Left, IN4_Left);
AccelStepper rightMotor(AccelStepper::FULL4WIRE, IN1_Right, IN2_Right, IN3_Right, IN4_Right);

void setup() {
    //Set motor max speed and desired acceleration
    leftMotor.setMaxSpeed(1000);
    rightMotor.setMaxSpeed(1000);
    leftMotor.setAcceleration(200);
    rightMotor.setAcceleration(200);
    leftMotor.setPinsInverted(true, true, false);
    rightMotor.setPinsInverted(true, true, false);
    // initialize the serial port:
    Serial.begin(9600);
    Serial1.begin(9600);
    bigMode = NONE;
    pinMode(2, INPUT);
}

void loop() {

    //State Machine (Overlord)
    switch(bigMode){
        //State Machine (Auto)
        case AUTO:
                switch(autoMode){
                //State 0
                case startWait:
                    //Wait for start command
                    //Go to next state when start command is received
                    readPI(input);
                    if(input == STOPCMD){
                        stopDriving();
                    }
                    else if(input == STARTCMD){
                        autoMode = target1;
                        input = "";
                        leftMotor.moveTo(420);
                        rightMotor.moveTo(420);
                        target2State = forward;
                        target3State = forward;
                    }
                    break;
                //State 1
                case target1:
                    //Drive forward approx 2 ft (554 steps per motor)
                    //Wait to receive confirmation that Target 1 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    readPI(input);
                    if(input == STOPCMD){
                        stopDriving();
                    }
                    else if (leftMotor.distanceToGo() != 0 && rightMotor.distanceToGo() != 0){
                        leftMotor.run();
                        rightMotor.run();
                    }
                    else if(input == TARGET1){
                        autoMode = target2;
                        input = "";
                        leftMotor.setCurrentPosition(0);
                        rightMotor.setCurrentPosition(0);
                        leftMotor.moveTo(480);
                        rightMotor.moveTo(480);
                    }
                    break;
                
                //State 2
                case target2:
                    //Sweeping turn to be in front of Target 2
                    //Wait to receive confirmation that Target 2 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    //Drive forward approx 2 ft (554 steps per motor)
                    //Wait to receive confirmation that Target 1 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    readPI(input);
                    if(input == STOPCMD){
                        stopDriving();
                    }
                    else if(target2State != turn_complete){
                        moveToTarget(target2State);
                    }
                    else if(input == TARGET2){
                        autoMode = target3;
                        input = "";
                        leftMotor.setCurrentPosition(0);
                        rightMotor.setCurrentPosition(0);
                        leftMotor.moveTo(420);
                        rightMotor.moveTo(420);
                    }
                    break;
                //State 3
                case target3:
                    //Sweeping turn to be in front of Target 3
                    //Wait to receive confirmation that Target 3 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    readPI(input);
                    if(input == STOPCMD){
                        stopDriving();
                    }
                    else if(target3State != turn_complete){
                        moveToTarget(target3State);
                    }
                    else if(input == TARGET3){
                        autoMode = target4;
                        input = "";
                        leftMotor.setCurrentPosition(0);
                        rightMotor.setCurrentPosition(0);
                        leftMotor.moveTo(95);
                    }
                    break;
                //State 4 
                case target4:
                    //Spin 45 degrees and move forward to be in front of the 4th target
                    readPI(input);
                    if(input == STOPCMD){
                        stopDriving();
                    }
                    else if(rightMotor.currentPosition() == 0 && leftMotor.currentPosition() == 86){
                        rightMotor.setCurrentPosition(0);
                        leftMotor.setCurrentPosition(0);
                        rightMotor.moveTo(500);
                        leftMotor.moveTo(500);
                        delay(500);
                    }
                    else if(rightMotor.distanceToGo() != 0 || leftMotor.distanceToGo() != 0){
                      leftMotor.run();
                      rightMotor.run();
                    }
                    else if(input == TARGET4){
                        autoMode = finishAUTO;
                        input = "";
                        leftMotor.moveTo(0);
                        rightMotor.moveTo(0);
                    }
                    
                    //Wait to receive confirmation that Target 4 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    break;
                //State 5 (Close Shot)
                //case target4:
                    //Drive to front of 4th target
                    //Wait to receive confirmation that Target 4 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    //break;
                //State 6 (Close Shot)
                case finishAUTO:
                    //Turn 180 degrees
                    //Drive forward forward_second to area of target 3
                    //Turn to face end zone
                    //Drive forward into end zone
                    readPI(input);
                    if(input == STOPCMD){
                        stopDriving();
                    }
                    else if(rightMotor.currentPosition() == 0 && leftMotor.currentPosition() == -70 && leftMotor.distanceToGo() == 0){
                        delay(500);
                        rightMotor.setCurrentPosition(0);
                        leftMotor.setCurrentPosition(0);
                        rightMotor.moveTo(600);
                        leftMotor.moveTo(600);
                    }
                    else if(leftMotor.distanceToGo() == 0 && rightMotor.currentPosition() == 0){
                        delay(500);
                        rightMotor.setCurrentPosition(0);
                        leftMotor.setCurrentPosition(0);
                        leftMotor.moveTo(-70);
                    }
                    else{
                      leftMotor.run();
                      rightMotor.run();
                    }
                }
        break;
        
    case MANUAL:
        //State Machine (Manual)
        //drive(2000, 2000, 40);
        //Serial.print("idk man. itll drive or some shit \n");
        //delay(5000);
        readPI(input);
        /*if(input == MOVERIGHT){
            rightMotor.setCurrentPosition(0);
            rightMotor.moveTo(5);
        }
        if(input == MOVELEFT){
            leftMotor.setCurrentPosition(0);
            leftMotor.moveTo(5);
        }
        if(input == MOVEBACK){
            leftMotor.setCurrentPosition(0);
            leftMotor.moveTo(-5);
            rightMotor.setCurrentPosition(0);
            rightMotor.moveTo(-5);
        }*/
        break;
    case NONE:
        readPI(input);
        if(input == AUTOCMD){
          bigMode = AUTO;
          autoMode = startWait;
        }
        else if(input == MANUALCMD){
          bigMode = MANUAL;
        }
        else{
          bigMode = NONE;
        }
        break;
    }
}

void readPI(String &input){
    if (Serial1.available() > 0){
        input = Serial1.readStringUntil('\n');
        Serial.print("R PI sent: ");
        Serial.print(input);
        Serial.print("\n");
    }
}

void moveToTarget(movementState &state){
  switch(state){
      case forward:
          if(leftMotor.distanceToGo() == 0 && rightMotor.distanceToGo() == 0){
              leftMotor.setCurrentPosition(0);
              rightMotor.setCurrentPosition(0);
              leftMotor.moveTo(185);
              state = turn;
              delay(500);
          }
          else{
              leftMotor.run();
              rightMotor.run();
          }
          break;
          case turn:
              if(leftMotor.distanceToGo() == 0 && rightMotor.distanceToGo() == 0){
                  leftMotor.setCurrentPosition(0);
                  rightMotor.setCurrentPosition(0);
                  leftMotor.moveTo(320);
                  rightMotor.moveTo(320);
                  state = forward_second;
                  delay(500);
             }
             else{
                 leftMotor.run();
                 rightMotor.run();
             }
             break;
             case forward_second:
             if(leftMotor.distanceToGo() == 0 && rightMotor.distanceToGo() == 0){
                state = turn_complete;
             }
             else{
                leftMotor.run();
                rightMotor.run();
             }
             break;
    }
}

void stopDriving(){
    bigMode = NONE;
    input = "";
    leftMotor.setCurrentPosition(0);
    rightMotor.setCurrentPosition(0);
    leftMotor.moveTo(0);
    rightMotor.moveTo(0);
    autoMode = startWait;
}
