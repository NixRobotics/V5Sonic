#include "vex.h"

using namespace vex;

void DriveCommands::MotorRampVoltage()
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

void DriveCommands::MotorRampRPM()
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

#define NUM_STEPS 1000

void DriveCommands::MotorStep()
{
  XYPlotter plot;

  SpeedControl leftSpeedControl;
  SpeedControl rightSpeedControl;

  float samples[NUM_STEPS][5];
  int sampleCount = 0;

  int rpm = 0;

  for (int i = 0; i <= NUM_STEPS; i++) {

    if (true) {
      if (i == 50) rpm = 50;
      if (i == 150) rpm = -50;
      if (i == 250) rpm = 100;
      if (i == 350) rpm = -100;
      if (i == 450) rpm = 300;
      if (i == 550) rpm = -300;
      if (i == 650) rpm = 0;
      if (i > 700) {
        float rpm_step = 300.0 / ((float) 300); // step size in RPM
        rpm = (int) (((float) (i - 700)) * rpm_step);
      }
    }
    if (false) {
      float rpm_step = 300.0 / ((float) NUM_STEPS); // step size in RPM
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

  for (int i = 0; i < NUM_STEPS; i++) {
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

bool DriveCommands::CheckTurnsDone(PositionControl &leftPositionControl, PositionControl &rightPositionControl)
{
  bool bLeftSettled = leftPositionControl.isSettled();
  bool bRightSettled = rightPositionControl.isSettled();
  bool bLeftTimedOut = leftPositionControl.isTimedOut();
  bool bRightTimedOut = rightPositionControl.isTimedOut();

  if (bLeftSettled && bRightSettled) {
    printf("Settled at %0.4f, %0.4f\n", LeftDrive.position(rotationUnits::rev), RightDrive.position(rotationUnits::rev));
    return true;
  } else if (bLeftTimedOut || bRightTimedOut) {
      printf("Timed out at %0.4f, %0.4f\n", LeftDrive.position(rotationUnits::rev), RightDrive.position(rotationUnits::rev));
    return true;
  }
  return false;
}

void DriveCommands::DriveFor()
{

  PositionControl leftPositionControl;
  PositionControl rightPositionControl;

  SpeedControl leftSpeedControl;
  SpeedControl rightSpeedControl;

  float samples[500][7];
  int sampleCount = 0;
  float revolutions = 0.0;

  for (int i = 0; i <= 500; i++) {

    if (true) {
      if (i == 50) {
        revolutions = 3.0 * 60.0 / 24.0;
        leftPositionControl.setPosition(revolutions);
        rightPositionControl.setPosition(revolutions);
      }
    }

    float left_rpm = leftPositionControl.getOutput(LeftDrive.position(rotationUnits::rev)) * 300.0;
    float right_rpm = rightPositionControl.getOutput(RightDrive.position(rotationUnits::rev)) * 300.0;

    if (CheckTurnsDone(leftPositionControl, rightPositionControl)) {
      leftPositionControl.reset();
      rightPositionControl.reset();
      leftSpeedControl.setStopping(true);
      rightSpeedControl.setStopping(true);
    }

    leftSpeedControl.setSpeed((int) left_rpm);
    rightSpeedControl.setSpeed((int) right_rpm);

    float left_voltage = leftSpeedControl.getOutput(LeftDrive.velocity(velocityUnits::rpm));
    float right_voltage = rightSpeedControl.getOutput(RightDrive.velocity(velocityUnits::rpm));

    LeftDrive.spin(forward, left_voltage, voltageUnits::volt);
    RightDrive.spin(forward, right_voltage, voltageUnits::volt);

    wait(10, msec);

    float left_position = LeftDrive.position(rotationUnits::rev);
    float right_position = RightDrive.position(rotationUnits::rev);

    float left_speed = LeftDrive.velocity(velocityUnits::rpm);
    float right_speed = RightDrive.velocity(velocityUnits::rpm);

    samples[i][0] = revolutions;
    samples[i][1] = left_position;
    samples[i][2] = right_position;
    samples[i][3] = left_rpm;
    samples[i][4] = right_rpm;
    samples[i][5] = left_speed;
    samples[i][6] = right_speed;
  }

  LeftDrive.stop(coast);
  RightDrive.stop(coast);

  for (int i = 0; i < 500; i++) {
    printf("%0.1f, %0.4f, %0.4f, %0.2f, %0.2f, %0.2f, %0.2f\n",
      samples[i][0],
      samples[i][1],
      samples[i][2],
      samples[i][3],
      samples[i][4],
      samples[i][5],
      samples[i][6]
      );
    wait(20, msec);
  }

}

void DriveCommands::MotorNoise()
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
