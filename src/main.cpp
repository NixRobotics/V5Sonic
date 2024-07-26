/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       nickr                                                     */
/*    Created:      11/26/2023, 10:21:21 AM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include <string>
#include <iostream>

using namespace vex;

// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

bool bLimitsSet = false;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {

  vexcodeInit();

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...

  IntakeArm.setMaxTorque(100.0, pct);
  IntakeArm.setPosition(0.0, deg);
  IntakeArm.setVelocity(50.0, pct);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

struct tSlingLog {
    int32_t time;
    float pos;
    float torque;
};

#define MAXLOG 20000
tSlingLog LOGARRAY[MAXLOG];
int32_t logId = 0;

void slingChanged(float pos, float torque)
{
    if (logId < MAXLOG) {
        LOGARRAY[logId].time = vex::timer::system();
        LOGARRAY[logId].pos = pos;
        LOGARRAY[logId].torque = torque;
        logId++;
    } else if (logId == MAXLOG) {
        printf("log buffer overflow\n");
        logId++;
    }
}

void DumpLog()
{
    if (!Brain.SDcard.isInserted()) {
        printf("No SDCARD\n");
    }
    if (logId >= MAXLOG) logId = MAXLOG;
    std::string str = "";
    char tempStr[100];
    str += "time, deg, torque\n";
    for (int i = 0; i < logId; i++) {
        sprintf(&(tempStr[0]), "%ld, %f, %f\n", LOGARRAY[i].time, LOGARRAY[i].pos, LOGARRAY[i].torque);
        str += tempStr;
        // str += LOGARRAY[i].time + LOGARRAY[i].val + "\n";
    }

    const char *cStr = str.c_str();
    int len = strlen(cStr);

    if (Brain.SDcard.isInserted()) {
        int saved = Brain.SDcard.savefile("data.csv", (uint8_t *) cStr, len);

        printf("%d of %d bytes written to file\n", saved, len);
        if (Brain.SDcard.exists("data.csv")) {
            printf("File size: %ld\n", Brain.SDcard.size("data.csv"));        }
    } else {
        printf("%s", cStr);
    }
    // printf("%s", str.c_str());
    // Brain.SDcard.savefile
}


// Main loop runrate, approx 25ms
int loopCount = 0;

void whenControllerAPressed() {
  if (bLimitsSet) return;

  IntakeArm.setPosition(0.0, deg);
  IntakePusher.setPosition(0.0, deg);
  GoalMotor.setPosition(0.0, deg);
  IntakeArm.setStopping(hold);

  bLimitsSet = true;
}

void whenControllerUpPressed()
{
  if (!bLimitsSet) return;

  GoalMotor.setStopping(hold);
  GoalMotor.setVelocity(100.0, pct);
  GoalMotor.setMaxTorque(100.0, pct);
  GoalMotor.spinTo(3.2 * 360.0, deg);
}

void whenControllerDownPressed()
{
  if (!bLimitsSet) return;

  GoalMotor.setStopping(hold);
  GoalMotor.setVelocity(100.0, pct);
  GoalMotor.setMaxTorque(100.0, pct);
  GoalMotor.spinTo(1.5 * 360.0, deg);
}

void whenControllerLeftPressed()
{
  if (!bLimitsSet) return;

  GoalMotor.setStopping(coast);
  GoalMotor.stop();
  //GoalMotor.setVelocity(100.0, pct);
  //GoalMotor.setMaxTorque(100.0, pct);
  //GoalMotor.spinTo(0.0 * 360.0, deg);
}

void whenControllerR1Pressed()
{
  if (!bLimitsSet) return;

  IntakePusher.setStopping(coast);
  IntakePusher.setVelocity(100.0, pct);
  IntakePusher.spinTo(0.0, deg, true);
  //GoalMotor.setMaxTorque(100.0, pct);
  //GoalMotor.spinTo(0.0 * 360.0, deg);
}

void whenControllerR2Pressed()
{
  if (!bLimitsSet) return;

  IntakePusher.setStopping(coast);
  IntakePusher.setVelocity(100.0, pct);
  IntakePusher.spinTo(-2.4 * 360.0, deg, true);
  //GoalMotor.setMaxTorque(100.0, pct);
  //GoalMotor.spinTo(0.0 * 360.0, deg);
}

void whenControllerYPressed() {
  ToggleDriveSpeed();
}


void ArmControl(double armrotraw)
{
    float deadband = CONTROLLER_DEADBAND;
    float gearratio = 84.0 / 12.0;
    float armdownlimit = -1.0;
    float armuplimit = 70.0;
    float armscale = 0.75;

    float armrot = 0.0;
    float armposition = IntakeArm.position(deg);
    if (armposition < armdownlimit * gearratio) {
      if (armrotraw > 0.0) armrotraw = 0.0;
    } else if (armposition > armuplimit * gearratio) {
      if (armrotraw < 0.0) armrotraw = 0.0;
    } else {
    }

    if (armrotraw > deadband) armrot = -(armrotraw - deadband) * (100.0 / (100.0 - deadband)) * armscale;
    else if (armrotraw < -deadband) armrot = -(armrotraw + deadband) * (100.0 / (100.0 - deadband)) * armscale;

    IntakeArm.spin(fwd, armrot, pct);
}

void usercontrol(void) {

  printf("user\n");

  while (!bLimitsSet) {
    wait(100, msec);
  }

  IntakeArm.spinTo(12.5 * 84.0 / 12.0, deg, true);

  // User control code here, inside the loop
  while (true) {
    float updown = Controller1.Axis3.position();
    float leftright = Controller1.Axis4.position();
    SimpleDrive(updown, leftright);
    // cheesyDrive(updown, leftright);

    float armrotraw = Controller1.Axis2.position();
    ArmControl(armrotraw);

    wait(25, msec);
    loopCount++;
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Install callbacks for buttons
  // Controller1.ButtonL1.pressed(whenControllerL1Pressed);
  // Controller1.ButtonL2.pressed(whenControllerL2Pressed);

  Controller1.ButtonR1.pressed(whenControllerR1Pressed);
  Controller1.ButtonR2.pressed(whenControllerR2Pressed);

  Controller1.ButtonA.pressed(whenControllerAPressed);

  Controller1.ButtonY.pressed(whenControllerYPressed);
  // Controller1.ButtonDown.pressed(whenControllerDownPressed);

  Controller1.ButtonUp.pressed(whenControllerUpPressed);
  Controller1.ButtonDown.pressed(whenControllerDownPressed);
  Controller1.ButtonRight.pressed(whenControllerLeftPressed);
  Controller1.ButtonLeft.pressed(whenControllerLeftPressed);

  // Controller1.ButtonA.pressed(whenControllerAPressed);
  
  printf("hello vex world\n");
  Brain.Screen.print("## SONIC ##");
  Brain.Screen.newLine();
  Brain.Screen.newLine();
  Brain.Screen.print("TO ENABLE ROBOT: CHECK POSITIONS THEN PRESS A");
  Brain.Screen.newLine();
  Brain.Screen.print(" - ARM IS DOWN");
  Brain.Screen.newLine();
  Brain.Screen.print(" - PUSHER IS UP");
  Brain.Screen.newLine();
  Brain.Screen.print(" - GOAL CATCH IS DOWN");

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("SEE BRAIN");

  while (!bLimitsSet) {
    wait(100, msec);
  }

  // Prevent main from exiting with an infinite loop.
  int loopcount = 0;
  while (true) {
    if (loopcount % 10 == 0) {
      int armtemp = (int) IntakeArm.temperature(pct);
      int pushtemp = (int) IntakePusher.temperature(pct);
      int goaltemp = (int) GoalMotor.temperature(pct);
      int motor1temp = (int) RightFrontMotor.temperature(pct);
      int motor2temp = (int) LeftFrontMotor.temperature(pct);
      int motor3temp = (int) RightRearMotor.temperature(pct);
      int motor4temp = (int) LeftRearMotor.temperature(pct);
      if (armtemp > 50 || pushtemp > 50 || goaltemp > 50 || motor1temp > 50 || motor2temp > 50 || motor3temp > 50 || motor4temp > 50)
      {
        Brain.Screen.clearScreen(orange);
        Brain.Screen.setFillColor(orange);
      }
      else if (armtemp >= 70 || pushtemp > 70 || goaltemp > 70 || motor1temp > 70 || motor2temp > 70 || motor3temp > 70 || motor4temp > 70)
      {
        Brain.Screen.clearScreen(red);
        Brain.Screen.setFillColor(red);
      } else {
        Brain.Screen.clearScreen(green);
        Brain.Screen.setFillColor(green);
      }
      Brain.Screen.setPenColor(black);
      Brain.Screen.setCursor(1, 1);
      Brain.Screen.print("## SONIC ##");
      Brain.Screen.setCursor(3, 1);
      Brain.Screen.print("Arm Temp: %3d", armtemp);
      Brain.Screen.setCursor(4, 1);
      Brain.Screen.print("Pusher Temp: %3d", pushtemp);
      Brain.Screen.setCursor(5, 1);
      Brain.Screen.print("Goal Temp: %3d", goaltemp);
      Brain.Screen.setCursor(6, 1);
      Brain.Screen.print("Drive Temps: %3d %3d %3d %3d", motor1temp, motor2temp, motor3temp, motor4temp);

      int batlevel =  (int) Brain.Battery.capacity(pct);
      Controller1.Screen.clearScreen();
      Controller1.Screen.setCursor(1, 1);
      Controller1.Screen.print("SONIC BAT: %2d", batlevel);

    }
    wait(100, msec);
    loopcount++;
  }
}
