# Arduino Firmware for NL2 Control Panel

## Overview

This is an Arduino Sketch file for my custom made physical control panel, built to simulate a basic control panel of a roller coaster station and to remotely control the NoLimits 2 Roller Coaster Simulator (NL2) software. It is only designed to be used with an Arduino "Leonardo" model microcontroller, which can emulate keyboard inputs to a computer over USB, which can then be used by NL2 via key binds for control. The operators of the control panel must also be wired in a specific configuration, specified in more detail below.

## Pinout for Operators

The following is a table of pinouts for the Arduino and what operator or lamp it should go to. Ground is shared. Buttons should be wired to their "pressed" circuit, unless otherwise specified. Lamps should be 5V LED bulbs.

| Pin Number | Operator / Lamp |
|------------|----------------------|
| 0 | Floor Button (Depressed) |
| 1 | E-Stop Button |
| 2 | E-Stop Lamp |
| 3 | Reset Button |
| 4 | Reset Lamp |
| 5 | Dispatch Button (Left) |
| 6 | Dispatch Lamp (Left) |
| 7 | Restraint Switch |
| 8 | Gates Switch |
| 9 | Key Switch |
| 10 | Floor Button |
| 11 | Floor Lamp |
| 12 | Dispatch Button (Right) |
| 13 | Dispatch Lamp (Right) |

## NL2 Keybinds

The following are the actions the Arduino will output as key presses, and therefore what keybinds should be set within NL2.

| Action | Keybind |
| ------ | ------- |
| Dispatch | `Return` |
| E-Stop | `F8` |
| Gates Open | `9` |
| Gates Close | `3` |
| Restraint Open | `5` |
| Restraint Close | `2` |
| Floor Lower | `1` |
| Floor Raise | `7` |

## Usage Instructions

### Preparation

1. Start the NL2 software, load a park, and navigate to the station of the coaster you want to control. Switch the station to Manual Dispatch Mode.
2. Ensure the Control Power key is switched to `Off`.
3. Plug in the control panel to the computer via USB.
4. If the coaster's station has a retracting floor or the coaster is a flying type, press & hold the Floor button until it flashes. This will enable the Floor button for use. Repeat (while Control Power is `Off`) to disable it.
5. Manually dispatch one train in the NL2 simulation and time, in seconds, how long it takes to dispatch (ie. leave the station). Press & hold the E-Stop Reset button until it lights up, then immediately release. Wait the same amount of time it takes to dispatch one train, and then press & hold the button again until it flashes. Because the panel cannot currently communicate with the NL2 software, this will set the dispatch time for proper internal timing.
6. Lastly, turn the Restraints and Gates switches to match the current state of restraints and gates in the simulation's station.

### Standard Startup Procedure

1. Turn the Control Power key to the `On` position. The E-Stop Reset button should light up, indicating there is power and that the lamp works.
2. Next, the safety systems are tested, as indicated by the E-Stop button light. For the power initialization of a real attraction, we would want to ensure it is started in a safe, passive, and/or E-Stopped state. Therefore, the E-Stop light will flash until E-Stop button is pressed in. This can be manually bypassed by pressing the E-Stop Reset button if desired.
3. Next, the station state is tested. By default, the panel prefers the restraints & gates to be closed and the floor/seats to be stowed (if applicable), ready for dispatch. If any of the switches are not in this default state, the left Dispatch button light will blink until the switches are switched to the default state or the left Dispatch button is pressed to bypass.
4. Once the station state is synced, the left and right Dispatch buttons will light up, indicating that the lamps work.
5. If the Floor button is enabled, it will light up, indicating that the lamp works.
6. The panel is now initialized for standard operation.

### Standard Operating Procedure

#### Load/Unload Cycle:

1. Open gates.
2. Lock restraints.
3. Close gates.
4. If applicable, lower the floor or raise the flyer seats.
5. Dispatch the train by pressing both Dispatch/Advance buttons simultaneously.
6. Wait for a new train to advance into the station.
7. If applicable, raise the floor or lower the flyer seats.
8. Unlock the restraints.

#### Emergency Stop + Recovery:

1. In event of an "emergency", press the E-Stop button to its lowered position. It will flash rapidly when enabled.
2. If desired, power can be cut by turning the Control Power key to `Off`.
3. [Recovery] When "safe", normal operation can be restored by first lifting the E-Stop button to its raised position.
4. The E-Stop Reset button will then light up. Press this button to reset and return to normal operation.
