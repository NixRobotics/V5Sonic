#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

// VEXcode device constructors
// NOTE: PUTT-PUTT PORT 19 is DEAD - do not use
controller Controller1 = controller(primary);
#ifdef USING_6MOTORS
motor L1 = motor(PORT4, ratio6_1, true);
motor L2 = motor(PORT5, ratio6_1, true);
motor L3 = motor(PORT6, ratio6_1, false);
motor_group LeftDrive = motor_group(L1, L2, L3);
motor R1 = motor(PORT1, ratio6_1, false);
motor R2 = motor(PORT2, ratio6_1, false);
motor R3 = motor(PORT3, ratio6_1, true);
motor_group RightDrive = motor_group(R1, R2, R3);
motor dtmotors[NUM_DTMOTORS] = {L1, L2, L3, R1, R2, R3};
#else
motor LeftFrontMotor = motor(PORT2, ratio6_1, true);
motor LeftRearMotor = motor(PORT4, ratio6_1, true);
motor_group LeftDrive = motor_group(LeftFrontMotor, LeftRearMotor);
motor RightFrontMotor = motor(PORT1, ratio6_1, false);
motor RightRearMotor = motor(PORT3, ratio6_1, false);
motor_group RightDrive = motor_group(RightFrontMotor, RightRearMotor);
motor dtmotors[NUM_DTMOTORS] = {LeftFrontMotor, LeftRearMotor, RightFrontMotor, RightRearMotor};
#endif
inertial InertialSensor = inertial(PORT10);
distance RightRearDistance = distance(PORT9);
distance LeftRearDistance = distance(PORT8);

float wheelbase = (7.0 + 7.0 + 6.5 + 6.5) * 0.5 * 25.4; // mm
float trackwidth = 320.0; //mm
smartdrive DriveTrain = smartdrive(LeftDrive, RightDrive, InertialSensor, 260.0, trackwidth, wheelbase, mm, 24.0 / 60.0);


// https://www.vexforum.com/t/vexcode-motor-groups-and-drivetrain-example/69161

// VEXcode generated functions

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void) {
  // nothing to initialize
  wait(100, msec);
  InertialSensor.calibrate();
  while (InertialSensor.isCalibrating()) {
    wait(20, msec);
  }
}