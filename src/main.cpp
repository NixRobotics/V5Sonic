/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       nickr                                                     */
/*    Created:      11/26/2023, 10:21:21 AM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"


using namespace vex;

// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

bool bInitialized = false;

void pre_auton(void) {

  vexcodeInit();

  wait(100, msec);

  bInitialized = true;

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...

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

  DriveTrain.setDriveVelocity(10.0, pct);
  DriveTrain.setTurnVelocity(10.0, pct);
  DriveTrain.setStopping(brake);

  float gyro_compensation = 360.0 / 365.5;

  DriveTrain.turnFor(360.0 * gyro_compensation, rotationUnits::deg, 10.0, velocityUnits::pct, true);

  DriveTrain.stop(coast);

  this_thread::sleep_for(1000);

  wait (1, seconds);

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


/* HACK: Moving average for N motors over 10 readings */
#define AVG_COUNT     NUM_DTMOTORS
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
  sum = (((sum / (float) AVG_READINGS)) / maxTrq) * 100.0;

  ptr[idx] = (ptr[idx] + 1) % AVG_READINGS;

  return (int) sum;

}


void usercontrol(void) {

  while (!bInitialized) {
    this_thread::sleep_for(10); // wait for pre_auton to finish
  }

  while (true) this_thread::sleep_for(1000); // wait for user to stop the motors

  int loopCount = 0;

  printf("user\n");

  LogInit();

  // User control code here, inside the loop
  while (true) {
    float updown = Controller1.Axis3.position();
    float leftright = Controller1.Axis4.position();

    double left_speed, right_speed;
    SimpleDrive(&left_speed, &right_speed, updown, leftright);
    // cheesyDrive(updown, leftright);


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
  
  UIPrintInitial();

  // MotorNoise();
  // MotorStep();
  DriveCommands::DriveFor();

  while(true) this_thread::sleep_for(1000);

  // Prevent main from exiting with an infinite loop.
  int loopcount = 0;
  while (true) {
    
    int dtmotor_trqs[NUM_DTMOTORS];
    int dtmotor_temp[NUM_DTMOTORS];

    for (int i = 0; i < NUM_DTMOTORS; i++) {
      float maxtrq = (NUM_DTMOTORS == 6) ? 1.05 / 3.0 : 1.05;
      dtmotor_trqs[i] = AvgTorque(1, dtmotors[i].torque(vex::torqueUnits::Nm), maxtrq);
    }

    if (loopcount % 100 == 0) {
      UIPrintCurrent(dtmotor_temp, dtmotor_trqs);
    }
    wait(10, msec);
    loopcount++;
  }
}
