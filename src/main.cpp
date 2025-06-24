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

void MotorRamp()
{
  for (int i = 0; i <= 100; i++) {
    float voltage = 12.0 * (float) i / 100.0;
    LeftDrive.spin(forward, voltage, voltageUnits::volt);
    RightDrive.spin(forward, voltage, voltageUnits::volt);
    for (int j = 0; j < 10; j++) {
      wait(40, msec);
      printf("%f, %f, %f\n", voltage, LeftDrive.velocity(velocityUnits::rpm), RightDrive.velocity(velocityUnits::rpm));
    }
  }
  LeftDrive.stop(coast);
  RightDrive.stop(coast);
}

void MotorRampRPM()
{
  XYPlotter plot;

  SpeedControl leftSpeedControl;
  SpeedControl rightSpeedControl;

  for (int i = 0; i <= 100; i++) {

    float rpm = 600.0 * (float) i / 100.0;

    leftSpeedControl.setSpeed(rpm);
    rightSpeedControl.setSpeed(rpm);

    float left_accum = 0.0;
    float right_accum = 0.0;

    for (int j = 0; j < 20; j++) {
      float left_voltage = leftSpeedControl.getOutput(LeftDrive.velocity(velocityUnits::rpm));
      float right_voltage = rightSpeedControl.getOutput(RightDrive.velocity(velocityUnits::rpm));

      LeftDrive.spin(forward, left_voltage, voltageUnits::volt);
      RightDrive.spin(forward, right_voltage, voltageUnits::volt);

      wait(10, msec);

      if (j % 2 == 1) {
        printf("%f, %f, %f\n", rpm, LeftDrive.velocity(velocityUnits::rpm), RightDrive.velocity(velocityUnits::rpm));
        left_accum += LeftDrive.velocity(velocityUnits::rpm);
        right_accum += RightDrive.velocity(velocityUnits::rpm);
      }

    }

    plot.addDataPointSeries1(rpm, left_accum / 10.0);
    plot.addDataPointSeries2(rpm, right_accum / 10.0);

  }
  LeftDrive.stop(coast);
  RightDrive.stop(coast);

  plot.drawPlot();

}

void MotorStep()
{
  XYPlotter plot;

  SpeedControl leftSpeedControl;
  SpeedControl rightSpeedControl;

  float samples[500][5];
  int sampleCount = 0;

  int rpm = 0;
  float rpm_step = 600.0 / 500.0; // step size in RPM

  for (int i = 0; i <= 500; i++) {

    if (true) {
      if (i == 100) rpm = 100;
      if (i == 200) rpm = -100;
      if (i == 300) rpm = 300;
      if (i == 400) rpm = -300;
    }
    if (false) {
      rpm = (int) (((float) i) * rpm_step);
    }

    leftSpeedControl.setSpeed(rpm);
    rightSpeedControl.setSpeed(rpm);

    float left_voltage = leftSpeedControl.getOutput(LeftDrive.velocity(velocityUnits::rpm));
    float right_voltage = rightSpeedControl.getOutput(RightDrive.velocity(velocityUnits::rpm));

    LeftDrive.spin(forward, left_voltage, voltageUnits::volt);
    RightDrive.spin(forward, right_voltage, voltageUnits::volt);

    wait(10, msec);

    float left_speed = LeftDrive.velocity(velocityUnits::rpm);
    float right_speed = RightDrive.velocity(velocityUnits::rpm);

    samples[i][0] = (float) rpm;
    samples[i][1] = left_voltage;
    samples[i][2] = right_voltage;
    samples[i][3] = left_speed;
    samples[i][4] = right_speed;

    plot.addDataPointSeries1(i, rpm);
    plot.addDataPointSeries2(i, left_speed);
    plot.addDataPointSeries3(i, right_speed);
  }

  LeftDrive.stop(coast);
  RightDrive.stop(coast);

  plot.drawPlot();

  for (int i = 0; i < 500; i++) {
    printf("%d, %0.1f, %0.1f, %0.2f, %0.2f\n",
      (int) samples[i][0],
      samples[i][1],
      samples[i][2],
      samples[i][3],
      samples[i][4]
      );
    wait(20, msec);
  }

}

void MotorNoise()
{
  float samples[100][6];
  int sampleCount = 0;

  LeftDrive.spin(forward, 4.0, voltageUnits::volt);

  for (int i = 0; i < 100; i++) {
    samples[i][0] = LeftFrontMotor.timestamp();
    samples[i][1] = LeftFrontMotor.velocity(velocityUnits::rpm);
    samples[i][2] = LeftFrontMotor.position(rotationUnits::rev);

    samples[i][3] = LeftRearMotor.timestamp();
    samples[i][4] = LeftRearMotor.velocity(velocityUnits::rpm);
    samples[i][5] = LeftRearMotor.position(rotationUnits::rev);

    wait(10, msec);
  }

  LeftDrive.stop(coast);

  for (int i = 0; i < 100; i++) {
    printf("%f, %f, %f, %f, %f, %f\n", samples[i][0], samples[i][1], samples[i][2], samples[i][3], samples[i][4], samples[i][5]);
    wait(20, msec);
  }

}

void usercontrol(void) {

  while (!bInitialized) {
    this_thread::sleep_for(10); // wait for pre_auton to finish
  }

  while (true) this_thread::sleep_for(1000); // wait for user to stop the motors

  int loopCount = 0;

  printf("user\n");

  DriveTrain.setDriveVelocity(10.0, pct);
  DriveTrain.setTurnVelocity(10.0, pct);
  DriveTrain.setStopping(brake);

  float gyro_compensation = 360.0 / 365.5;

  DriveTrain.driveFor(reverse, 200.0, distanceUnits::mm, 12.5, velocityUnits::pct, true);

  // DriveTrain.turnFor(360.0 * gyro_compensation, rotationUnits::deg, 10.0, velocityUnits::pct, true);

//  DriveTrain.driveFor(forward, 200.0, distanceUnits::mm, 10.0, velocityUnits::pct, true);
  DriveTrain.stop(coast);

  this_thread::sleep_for(1000);

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
  MotorStep();

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
