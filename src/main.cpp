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

  wait (1, seconds);

  IntakeArm.setStopping(hold);

  bLimitsSet = true;

  DriveTrain.setDriveVelocity(33.0, pct);
  DriveTrain.setStopping(brake);
  DriveTrain.turnFor(360.0, deg, true);
  wait (0.10, seconds);
  DriveTrain.turnFor(-360.0, deg, true);
  wait (0.10, seconds);
  DriveTrain.driveFor(24.0, inches, true);
  wait (0.10, seconds);
  DriveTrain.turnFor(90.0, deg, true);
  wait (0.10, seconds);
  DriveTrain.driveFor(24.0, inches, true);
  wait (0.10, seconds);
  DriveTrain.turnFor(90.0, deg, true);
  wait (0.10, seconds);
  DriveTrain.driveFor(24.0, inches, true);
  wait (0.10, seconds);
  DriveTrain.turnFor(90.0, deg, true);
  wait (0.10, seconds);
  DriveTrain.driveFor(24.0, inches, true);
  wait (0.10, seconds);
  DriveTrain.turnFor(90.0, deg, true);
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

/* HACK: Moving average for 4 motors over 10 readings */
#define AVG_COUNT     4
#define AVG_READINGS  10
bool bAvgInitialized = false;

int AvgTorque(int id, float curTrq, float maxTrq = 1.05)
{
  int idx = id - 1; // make zero based
  static float readings[AVG_COUNT][AVG_READINGS];
  static int ptr[AVG_COUNT];

  if (idx < 0 || idx >= AVG_COUNT) return -1;

  if (!bAvgInitialized) {
    for (int i = 0; i < AVG_COUNT; i++) ptr[i] = -1;
    bAvgInitialized = true;
  }

  if (ptr[idx] == -1) {
    for (int i = 0; i < AVG_READINGS; i++) {
      readings[idx][i] = curTrq;
    }
    ptr[idx] = 0;
  } else {
    readings[idx][ptr[idx]] = curTrq;
  }

  float sum = 0.0;
  for (int i = 0; i < AVG_READINGS; i++) {
    int j = (ptr[idx] + 1 + i) % AVG_READINGS;
    sum += readings[idx][j];
  }
  sum = ((sum / (float) AVG_READINGS) / maxTrq) * 100.0;

  ptr[idx] = (ptr[idx] + 1) % AVG_READINGS;

  return (int) sum;

}

void usercontrol(void) {

  printf("user\n");

  while (!bLimitsSet) {
    wait(100, msec);
  }
  DriveTrain.setDriveVelocity(100.0, pct);
  DriveTrain.setStopping(coast);
  IntakeArm.spinTo(12.5 * 84.0 / 12.0, deg, true);
  LogInit();

  // User control code here, inside the loop
  while (true) {
    float updown = Controller1.Axis3.position();
    float leftright = Controller1.Axis4.position();

    double left_speed, right_speed;
    SimpleDrive(&left_speed, &right_speed, updown, leftright);
    // cheesyDrive(updown, leftright);

    float armrotraw = Controller1.Axis2.position();
    ArmControl(armrotraw);

    LogAccumState(left_speed, right_speed);
    if (loopCount % STATSLOOPMAX == 0) LogCalcStats();

    wait(10, msec);
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
  Brain.Screen.print("## YOSHI ##");
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
    
    int motor1trq = AvgTorque(1, RightFrontMotor.torque(vex::torqueUnits::Nm));
    int motor2trq = AvgTorque(2, RightRearMotor.torque(vex::torqueUnits::Nm));
    int motor3trq = AvgTorque(3, LeftFrontMotor.torque(vex::torqueUnits::Nm));
    int motor4trq = AvgTorque(4, LeftRearMotor.torque(vex::torqueUnits::Nm));

    if (loopcount % 100 == 0) {
      int armtemp = (int) IntakeArm.temperature(pct);
      int pushtemp = (int) IntakePusher.temperature(pct);
      int goaltemp = (int) GoalMotor.temperature(pct);
      int motor1temp = (int) RightFrontMotor.temperature(pct);
      int motor2temp = (int) RightRearMotor.temperature(pct);
      int motor3temp = (int) LeftFrontMotor.temperature(pct);
      int motor4temp = (int) LeftRearMotor.temperature(pct);

      int driveTemp = 0;
      if (motor1temp > 50 || motor2temp > 50 || motor3temp > 50 || motor4temp > 50) driveTemp = 1;
      else if (motor1temp > 70 || motor2temp > 70 || motor3temp > 70 || motor4temp > 70) driveTemp = 2;

      int armTemp = 0;
      if (armtemp > 50 || pushtemp > 50 || goaltemp > 50) armTemp = 1;
      else if (armtemp >= 70 || pushtemp > 70 || goaltemp > 70) armTemp = 2;

      if (driveTemp == 1 || armTemp == 1)
      {
        Brain.Screen.clearScreen(orange);
        Brain.Screen.setFillColor(orange);
      }
      else if (driveTemp == 2 || armTemp == 2)
      {
        Brain.Screen.clearScreen(red);
        Brain.Screen.setFillColor(red);
      } else {
        Brain.Screen.clearScreen(green);
        Brain.Screen.setFillColor(green);
      }
      Brain.Screen.setPenColor(black);
      Brain.Screen.setCursor(1, 1);
      Brain.Screen.print("## YOSHI ##");
      Brain.Screen.setCursor(3, 1);
      Brain.Screen.print("Arm Temp: %3d", armtemp);
      Brain.Screen.setCursor(4, 1);
      Brain.Screen.print("Pusher Temp: %3d", pushtemp);
      Brain.Screen.setCursor(5, 1);
      Brain.Screen.print("Goal Temp: %3d", goaltemp);
      Brain.Screen.setCursor(6, 1);
      Brain.Screen.print("Drive Temps: %3d %3d %3d %3d", motor1temp, motor2temp, motor3temp, motor4temp);
      Brain.Screen.setCursor(7, 1);
      Brain.Screen.print("Drive Torxs: %3d %3d %3d %3d", motor1trq, motor2trq, motor3trq, motor4trq);

      int batlevel =  (int) Brain.Battery.capacity(pct);
      Controller1.Screen.clearScreen();
      Controller1.Screen.setCursor(1, 1);
      Controller1.Screen.print("YOSHI BAT: %2d", batlevel);
      Controller1.Screen.setCursor(2, 1);
      if (driveTemp == 0) Controller1.Screen.print("DRIVE TEMP: OK");
      else if (driveTemp == 1) Controller1.Screen.print("DRIVE TEMP: WARM");
      else if (driveTemp == 2) Controller1.Screen.print("DRIVE TEMP: HOT");
      Controller1.Screen.setCursor(3, 1);
      if (armTemp == 0) Controller1.Screen.print("ARM TEMP: OK");
      else if (armTemp == 1) Controller1.Screen.print("ARM TEMP: WARM");
      else if (armTemp == 2) Controller1.Screen.print("ARM TEMP: HOT");

    }
    wait(10, msec);
    loopcount++;
  }
}
