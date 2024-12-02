/**
	NoLimits 2 (NL2) Physical Control Panel Emulator
	David Wolfe
	11/23/2024
    For use on Arduino Leonardo only.
	Reads the states of the operators wired to specific pins, controls indicator lamps,
        and emulates keyboard presses to mimic input to NL2 simulation.
    Operators: 
        Key Switch, Floor Button, Reset Button, E-Stop Button (Toggle)
        Dispatch Button (Left), Restraints Switch, Gates Switch, Dispatch Button (Right)
	Licensed under GNU GPLv3 - See LICENSE for more details.
*/

#include <Keyboard.h>;

// Set Pin Values for Buttons and Lights
const int FloorBTup = 0;
const int EstopBT = 1;
const int EstopLT = 2;
const int ResetBT = 3;
const int ResetLT = 4;
const int Dispatch1BT = 5;
const int Dispatch1LT = 6;
const int RestraintBT = 7;
const int GatesBT = 8;
const int KeyBT = 9;
const int FloorBT = 10;
const int FloorLT = 11;
const int Dispatch2BT = 12;
const int Dispatch2LT = 13;

// Set initial variable states.
int DispatchON = HIGH;      // Dispatch light on when program begins
int EstopON = LOW;          // Estop light off when program begins
int FloorON = LOW;          // Floor light off when program begins
int FloorR = LOW;           // Variable for Floor
int FloorNOW = LOW;         // Variable for current Floor value
int FloorST = LOW;          // Current Floor button variable
int FloorHX = LOW;          // Variable for Gate history value
int GateHX;                 // Variable for Gate history value
int RestraintHX;            // Variable for Restraint history value
int FMode = LOW;            // Variable for activating floor functions
int EstopHX = HIGH;         // Estop not activated; By default, previously "was" activated
int RCSEnable = HIGH;       // Variable for enabling RCS to run in normal mode
boolean DisRDY = false;     // Variable for determining if train is ready for dispatch
int StartUp = LOW;          // For light start up sequence

// Blink timer variable setup.
long previousMillis = 0;    // Starts counter at 0
long EpreviousMillis = 0;
long RpreviousMillis = 0;
long DispatchBLK = 700;     // # of milliseconds between each Dispatch1LT blink (1000ms = 1s)
long EstopBLK = 250;        // # of milliseconds between each EstopLT blink (1000ms = 1s)
long StatusBLK = 700;       // # of milliseconds between each Status Indicator Light blink (1000ms = 1s)
long RStatusBLK = 700;

void setup(){
    /* Set Pin Modes for Buttons and Lights */

    // Lights: OUTPUT
    pinMode(ResetLT, OUTPUT);
    pinMode(EstopLT, OUTPUT);
    pinMode(Dispatch1LT, OUTPUT);
    pinMode(FloorLT, OUTPUT);
    pinMode(Dispatch2LT, OUTPUT);

    // Buttons: INPUT_PULLUP  
    pinMode(ResetBT, INPUT_PULLUP);
    pinMode(EstopBT, INPUT_PULLUP);
    pinMode(Dispatch1BT, INPUT_PULLUP);
    pinMode(FloorBT, INPUT_PULLUP);
    pinMode(GatesBT, INPUT_PULLUP);
    pinMode(RestraintBT, INPUT_PULLUP);
    pinMode(KeyBT, INPUT_PULLUP);
    pinMode(Dispatch2BT, INPUT_PULLUP);
    pinMode(FloorBTup, INPUT_PULLUP);

    // Initialize control over the keyboard.
    Keyboard.begin();
}

void loop(){
    /* MAIN PROGRAM LOOP */

    // Set variable names for Button position states.
    int ResetPOS = digitalRead(ResetBT);
    int EstopPOS = digitalRead(EstopBT);
    int DispatchPOS = digitalRead(Dispatch1BT);
    int Dispatch2POS = digitalRead(Dispatch2BT);
    int FloorPOS = digitalRead(FloorBT);
    int GatePOS = digitalRead(GatesBT);
    int RestraintPOS = digitalRead(RestraintBT);
    int FloorUPPOS = digitalRead(FloorBTup);
    int KeyPOS = digitalRead(KeyBT);

    // Set variable for blink counter for blinking lights.
    unsigned long currentMillis = millis();
    unsigned long EcurrentMillis = millis();
    unsigned long RcurrentMillis = millis();

    if (KeyPOS == HIGH){ // If power switch in OFF position turn off all lights and reset variables
        digitalWrite(EstopLT, LOW);
        digitalWrite(ResetLT, LOW);
        digitalWrite(Dispatch1LT, LOW);
        digitalWrite(FloorLT, LOW);
        digitalWrite(Dispatch2LT, LOW);
        delay(500);
        StartUp = LOW;
        FMode = LOW;
        DisRDY = false;
        FloorR = LOW;
        RCSEnable = HIGH; // Re-enable RCS
        EstopHX = EstopPOS; // Update Estop history  
    }

    if (KeyPOS == LOW){ // If power switch in ON position  
        if (StartUp == LOW){
            delay(500);
            digitalWrite(FloorLT, HIGH);
            delay(500);
            digitalWrite(ResetLT, HIGH);
            delay(500);
            digitalWrite(EstopLT, HIGH);
            delay(500);
            digitalWrite(Dispatch1LT, HIGH);
            delay(500);
            digitalWrite(Dispatch2LT, HIGH);
            delay(500);
            digitalWrite(EstopLT, LOW);
            digitalWrite(ResetLT, LOW);
            digitalWrite(Dispatch1LT, LOW);
            digitalWrite(FloorLT, LOW);
            digitalWrite(Dispatch2LT, LOW);
            StartUp = HIGH;
        }

        // ==================== RCS Mode: Enabled ====================
        if (RCSEnable == HIGH && EstopPOS == HIGH){ // In Enabled RCS Mode anytime RCSEnable variable is high and Estop button position is high

            // Blinking Dispatch/Estop/Floor Light Function
            if (currentMillis - previousMillis > DispatchBLK){
                previousMillis = currentMillis; // Initializes timer with new value
                if (DispatchON == LOW){ // If dispatch light is off...
                    DispatchON = HIGH; // Turn on...
                }
                else // If dispatch light is on...
                    DispatchON = LOW; // Turn off...
            }

            digitalWrite(EstopLT, !DispatchON); // blink Estop opposite of Dispatch lights

            // Toggle Functions for changing Floor Mode
            if (DisRDY == true){
                if (ResetPOS == LOW){ // If reset button pressed and floor functions off, activate floor functions
                    digitalWrite(ResetLT, HIGH);
                    delay(250);
                    digitalWrite(ResetLT, LOW);
                    delay(250);
                    digitalWrite(ResetLT, HIGH);
                    delay(250);
                    digitalWrite(ResetLT, LOW);
                    delay(250);
                    digitalWrite(ResetLT, HIGH);
                    delay(250);
                    digitalWrite(ResetLT, LOW);
                    digitalWrite(FloorLT, LOW);
                    FMode = !FMode;
                    ResetPOS = digitalRead(ResetBT);
                }
            }

            // If Restraint, Gates, or Floor is open train not ready for dispatch
            if (RestraintPOS == HIGH){
                DisRDY = false;
            } else if (GatePOS == HIGH){
                DisRDY = false;
            } else if (FloorR == HIGH){
                DisRDY = false;
            } else
                DisRDY = true;

            if (DisRDY == false){
                digitalWrite(Dispatch1LT, LOW);
                digitalWrite(Dispatch2LT, LOW);
            }

            if (DisRDY == true){
                digitalWrite(Dispatch1LT, DispatchON);
                digitalWrite(Dispatch2LT, DispatchON);
            }

            if (FMode == LOW){ // Floor Mode Off
                // Restraint Function Control
                if (RestraintHX != RestraintPOS){ // If restraint button has moved...
                    delay(100); // Delay 100ms
                    if (RestraintPOS == LOW){ // If restraint button in close position...
                        Keyboard.press('2'); // Press 2 Key
                        delay(100); // Delay 100ms
                        Keyboard.releaseAll(); // Release 2 Key
                        RestraintHX = RestraintPOS;
                    } // Save new button value
                    else { // If restraint button in open position...
                        Keyboard.press('5'); // Press 5 Key
                        delay(100); // Delay 100ms
                        Keyboard.releaseAll(); // Release 5 Key
                        RestraintHX = RestraintPOS;
                    } // Save new button value  
                }
                // Gate Function Control
                if (GateHX != GatePOS){ // If gate button has moved...
                    delay(100); // Delay 100ms
                    if (GatePOS == LOW){ // If gate button in close position...
                        Keyboard.press('3'); // Press 3 Key
                        delay(100); // Delay 100ms
                        Keyboard.releaseAll(); // Release 3 Key
                        GateHX = GatePOS;
                    } // Save new button value
                    else { // If gate button in open position...
                        Keyboard.press('9'); // Press 9 Key
                        delay(100); // Delay 100ms
                        Keyboard.releaseAll(); // Release 9 Key
                        GateHX = GatePOS;
                    } // Save new button value
                }

                // Dispatch Function Control
                if (DisRDY == true){
                    if (DispatchPOS == LOW && Dispatch2POS == LOW){ // If dispatch button has been pressed...
                        Keyboard.press(KEY_RETURN); // Press Return (Enter) Key
                        delay(100); // Delay 100ms
                        Keyboard.releaseAll(); // Release Return (Enter) Key
                        digitalWrite(Dispatch1LT, HIGH); // Light up dispatch light
                        digitalWrite(Dispatch2LT, HIGH); // Light up dispatch light
                        delay(1000);
                    }
                } // Delay with dispatch light on for 1000ms (1s)
            }

            if (FMode == HIGH){ // Floor Mode On
                // used to blink floor light at EstopBLK speed if gate/restraint opened without lowered floor
                if (currentMillis - EpreviousMillis > EstopBLK){
                    EpreviousMillis = EcurrentMillis; // Initializes timer with new value
                    if (FloorON == LOW){ // If Estop light is off...
                        FloorON = HIGH;
                    } // Turn on...
                    else // If Estop light is on...
                        FloorON = LOW; // Turn off...
                    digitalWrite(FloorLT, FloorON);
                }

                if (FloorR == LOW){ // if floor not raised...
                    if (GatePOS == HIGH || RestraintPOS == HIGH){ // if gate or restraint opened...
                        digitalWrite(FloorLT, FloorON);
                    } // blink floor light at estopped speed
                    if (GatePOS == LOW && RestraintPOS == LOW){
                        digitalWrite(FloorLT, DispatchON);
                    } // else blink with dispatch
                }

                if (FloorR == HIGH){
                    digitalWrite(FloorLT, HIGH);
                } // solid floor light if floor raised

                if (GatePOS == LOW && RestraintPOS == LOW){
                    // Floor Function Control
                    if (FloorUPPOS == HIGH && FloorST == HIGH){ // Determine when floor button is pressed then toggle variable.
                        FloorPOS = digitalRead(FloorBT);
                        if (FloorPOS == LOW){
                            if (FloorNOW == HIGH){
                                FloorNOW = LOW;
                                FloorST = LOW;
                            } // Close position
                            else {
                                FloorNOW = HIGH;
                                FloorST = LOW;
                            } // Open position
                        }
                    }

                    if (FloorUPPOS == LOW){
                        FloorST = HIGH;
                    }

                    if (FloorHX != FloorNOW){ // If gate button has moved...
                        delay(100); // Delay 100ms
                        if (FloorNOW == LOW){ // If floor button in lower position...
                            Keyboard.press('1'); // Press 1 Key
                            delay(100); // Delay 100ms
                            Keyboard.releaseAll(); // Release 1 Key
                            FloorR = LOW;
                            FloorHX = FloorNOW;
                        } // Save new button value
                        else { // If floor button in raise position...
                            Keyboard.press('7'); // Press 7 Key
                            delay(100); // Delay 100ms
                            Keyboard.releaseAll(); // Release 7 Key
                            FloorR = HIGH;
                            FloorHX = FloorNOW;
                        } // Save new button value
                    }
                }

                if (FloorR == HIGH){
                    // Restraint Function Control
                    if (RestraintHX != RestraintPOS){ // If restraint button has moved...
                        delay(100); // Delay 100ms
                        if (RestraintPOS == LOW){ // If restraint button in close position...
                            Keyboard.press('2'); // Press 2 Key
                            delay(100); // Delay 100ms
                            Keyboard.releaseAll(); // Release 2 Key
                            RestraintHX = RestraintPOS;
                        } // Save new button value
                        else { // If restraint button in open position...
                            Keyboard.press('5'); // Press 5 Key
                            delay(100); // Delay 100ms
                            Keyboard.releaseAll(); // Release 5 Key
                            RestraintHX = RestraintPOS;
                        } // Save new button value  
                    }
                    // Gate Function Control
                    if (GateHX != GatePOS){ // If gate button has moved...
                        delay(100); // Delay 100ms
                        if (GatePOS == LOW){ // If gate button in close position...
                            Keyboard.press('3'); // Press 3 Key
                            delay(100); // Delay 100ms
                            Keyboard.releaseAll(); // Release 3 Key
                            GateHX = GatePOS;
                        } // Save new button value
                        else { // If gate button in open position...
                            Keyboard.press('9'); // Press 9 Key
                            delay(100); // Delay 100ms
                            Keyboard.releaseAll(); // Release 9 Key
                            GateHX = GatePOS;
                        } // Save new button value
                    }
                }

                // Dispatch Function Control
                if (DisRDY == true){
                    if (DispatchPOS == LOW && Dispatch2POS == LOW){ // If dispatch button has been pressed...
                        Keyboard.press(KEY_RETURN); // Press Return (Enter) Key
                        delay(100); // Delay 100ms
                        Keyboard.releaseAll(); // Release Return (Enter) Key
                        digitalWrite(Dispatch1LT, HIGH); // Light up dispatch light
                        digitalWrite(Dispatch2LT, HIGH); // Light up dispatch light
                        digitalWrite(FloorLT, LOW);
                        delay(1000);
                    }
                } // Delay with dispatch light on for 1000ms (1s) 
            }

        }

        // ==================== RCS Mode: E-Stop ====================
        if (EstopPOS == LOW){ // If E-Stop button is pressed...
            digitalWrite(ResetLT, LOW); // Turn off Reset light
            digitalWrite(Dispatch1LT, LOW); // Turn off Dispatch1 light
            digitalWrite(Dispatch2LT, LOW); // Turn off Dispatch2 light
            digitalWrite(FloorLT, LOW); // Turn off Floor light
            RCSEnable = LOW; // Deactivate Remote Control System

            // If pressed for the first time...
            if (EstopPOS == LOW && EstopHX == HIGH){
                Keyboard.press(KEY_F8); // Press F8 Key
                digitalWrite(EstopLT, HIGH); // Turn on E-Stop light
                EstopHX = EstopPOS; // Update E-Stop History
                delay(100); // Delay 100ms
                Keyboard.releaseAll(); // Release F8 Key
            }

            // Blinking E-Stop Light Function (E-Stop blinks fast when pressed down)
            if (currentMillis - previousMillis > EstopBLK){
                previousMillis = currentMillis; // Initializes timer with new value
                if (EstopON == LOW){ // If E-Stop light is off...
                    EstopON = HIGH; // Turn on...
                }
                else // If Estop light is on...
                    EstopON = LOW; // Turn off...
                digitalWrite(EstopLT, EstopON); // Write to E-Stop light to turn on/off
            }
        }

        // ==================== RCS Mode: Reset ====================
        if (EstopPOS == HIGH && RCSEnable == LOW){ // If E-Stop button is up and RCS is disabled...
            digitalWrite(ResetLT, HIGH); // Turn on E-Stop light
            digitalWrite(EstopLT, HIGH); // Turn on Reset light

            if (ResetPOS == LOW && EstopPOS == HIGH && RCSEnable == LOW){ // If reset button is pressed...
                RCSEnable = HIGH; // Re-enable RCS
                EstopHX = EstopPOS; // Update E-Stop history

                digitalWrite(ResetLT, LOW);
                delay(250);
                digitalWrite(ResetLT, HIGH);
                delay(250);
                digitalWrite(ResetLT, LOW);
                delay(250);
                digitalWrite(ResetLT, HIGH);
                delay(250);
                digitalWrite(ResetLT, LOW);
                delay(250);
                digitalWrite(ResetLT, HIGH);
                delay(250);
                digitalWrite(ResetLT, LOW);
                delay(250);
                digitalWrite(ResetLT, HIGH);

                Keyboard.press(KEY_F8); // Press S Key 
                delay(100); // Delay 100ms
                Keyboard.releaseAll(); // Release S Key
                digitalWrite(EstopLT, LOW); // E-Stop light off
                digitalWrite(ResetLT, LOW); // Reset light off
            }
        }
    }
}