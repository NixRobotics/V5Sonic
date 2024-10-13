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
motor LeftFrontMotor = motor(PORT2, ratio18_1, false);
motor LeftRearMotor = motor(PORT4, ratio18_1, false);
motor_group LeftDrive = motor_group(LeftFrontMotor, LeftRearMotor);
motor RightFrontMotor = motor(PORT1, ratio18_1, true);
motor RightRearMotor = motor(PORT3, ratio18_1, true);
motor_group RightDrive = motor_group(RightFrontMotor, RightRearMotor);
motor dtmotors[NUM_DTMOTORS] = {LeftFrontMotor, LeftRearMotor, RightFrontMotor, RightRearMotor};
#endif
drivetrain DriveTrain = drivetrain(LeftDrive, RightDrive, 4.15625 * M_PI, 15.0, 3.0, inches, 1.0);
motor GoalMotor = motor(PORT20, ratio18_1, false);
#define YOSHI
#ifdef YOSHI
motor IntakePusher = motor(PORT19, ratio18_1, true);
motor IntakeArm = motor(PORT18, ratio36_1, true);
#else
motor IntakePusher = motor(PORT18, ratio18_1, true);
motor IntakeArm = motor(PORT17, ratio36_1, false);
#endif
motor ConveyorMotor = motor(PORT15, ratio36_1, true);

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
}