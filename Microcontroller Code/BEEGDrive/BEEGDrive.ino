#include <AccelStepper.h>
#include "BEEGDrive.h"

enum mode { AUTO, MANUAL, NONE };
enum automaticMode { startWait, target1, target2, target3, target4, finishAUTO };
enum movementState { forward, turn, back };
String input = "NULL";

mode bigMode = AUTO;
automaticMode autoMode = startWait;
movementState target2State = forward;
movementState target3State = forward;

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
}

void loop() {

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
                    
                    delay(1000);
                    readPI(input);
                    if(input == STARTCMD){
                        autoMode = target1;
                        input = "";
                        leftMotor.moveTo(400);
                        rightMotor.moveTo(400);
                    }
                    else if(input == STOPCMD){
                        bigMode = NONE;
                    }
                    break;
            
                //State 1
                case target1:
                    //Drive forward approx 2 ft (554 steps per motor)
                    //Wait to receive confirmation that Target 1 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    readPI(input);
                    if (leftMotor.distanceToGo() != 0 && rightMotor.distanceToGo() != 0){
                        leftMotor.run();
                        rightMotor.run();
                    }
                    else if(input == TARGET1){
                        autoMode = target2;
                        input = "NULL";
                        leftMotor.setCurrentPosition(0);
                        rightMotor.setCurrentPosition(0);
                        leftMotor.moveTo(400);
                        rightMotor.moveTo(400);
                    }
                    else if(input == STOPCMD){
                        bigMode = NONE;
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
                    switch(target2State){
                        case forward:
                            if(leftMotor.distanceToGo() == 0 && rightMotor.distanceToGo() == 0){
                                leftMotor.setCurrentPosition(0);
                                rightMotor.setCurrentPosition(0);
                                leftMotor.moveTo(160);
                                target2State = turn;
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
                                leftMotor.moveTo(400);
                                rightMotor.moveTo(400);
                                target2State = back;
                                delay(500);
                            }
                            else{
                                leftMotor.run();
                                rightMotor.run();
                            }
                            break;
                        case back:
                            if(leftMotor.distanceToGo() == 0 && rightMotor.distanceToGo() == 0){
                                if(input == TARGET2){
                                    autoMode = target3;
                                    input = "";
                                    leftMotor.setCurrentPosition(0);
                                    rightMotor.setCurrentPosition(0);
                                    leftMotor.moveTo(400);
                                    rightMotor.moveTo(400);
                                }
                                else if(input == STOPCMD){
                                    bigMode = NONE;
                                }
                            }
                            else{
                                leftMotor.run();
                                rightMotor.run();
                            }
                            break;
                    }
                    break;
                //State 3
                case target3:
                    //Sweeping turn to be in front of Target 3
                    //Wait to receive confirmation that Target 3 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    readPI(input);
                    switch(target3State){
                        case forward:
                            if(leftMotor.distanceToGo() == 0 && rightMotor.distanceToGo() == 0){
                                leftMotor.setCurrentPosition(0);
                                rightMotor.setCurrentPosition(0);
                                leftMotor.moveTo(160);
                                target3State = turn;
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
                                leftMotor.moveTo(400);
                                rightMotor.moveTo(400);
                                target3State = back;
                                delay(500);
                            }
                            else{
                                leftMotor.run();
                                rightMotor.run();
                            }
                            break;
                        case back:
                            if(leftMotor.distanceToGo() == 0 && rightMotor.distanceToGo() == 0){
                                if(input == TARGET3){
                                    autoMode = target4;
                                    input = "";
                                    leftMotor.setCurrentPosition(0);
                                    rightMotor.setCurrentPosition(0);
                                }
                                else if(input == STOPCMD){
                                    bigMode = NONE;
                                }
                            }
                            else{
                                leftMotor.run();
                                rightMotor.run();
                            }
                            break;
                    }
                    break;
                    
                //State 4 (Long Shot)
                case target4:
                    Serial.print("onwards");
                    delay(1000);
                    //Wait to receive confirmation that Target 4 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    break;
                //State 4 (Close Shot)
                //case target4:
                    //Drive to front of 4th target
                    //Wait to receive confirmation that Target 4 has been hit
                    //Goes to next state upon receiving the confirmation from PI
                    //break;
                //State 5 (Long Shot)
                case finishAUTO:
                    //Drive forward into end zone
                    break;
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
        //drive(2000, 2000, 40);
        delay(5000);
        break;
    default:
        //if(readPI(input) == AUTOCMD){
        //bigMode = AUTO;
        //autoMode = startWait;
        //}
        //else if (readPI(input) == MANUALCMD){
            //bigMode = MANUAL;
        //}
        //else{
            //bigMode = NONE;
        //}
        break;
    }
}

void readPI(String &input){
    if (Serial.available() > 0){
        input = Serial.readStringUntil('\n');
        Serial.print("you sent: ");
        Serial.print(input);
        Serial.print('\n');
    }
}
