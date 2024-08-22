#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

// VEXcode device constructors
// NOTE: PUTT-PUTT PORT 19 is DEAD - do not use
controller Controller1 = controller(primary);
motor LeftFrontMotor = motor(PORT2, ratio6_1, true);
motor LeftRearMotor = motor(PORT4, ratio6_1, true);
motor_group LeftDrive = motor_group(LeftFrontMotor, LeftRearMotor);
motor RightFrontMotor = motor(PORT1, ratio6_1, false);
motor RightRearMotor = motor(PORT3, ratio6_1, false);
motor_group RightDrive = motor_group(RightFrontMotor, RightRearMotor);
drivetrain DriveTrain = drivetrain(LeftDrive, RightDrive, 4.15625 * M_PI, 15.0, 3.0, inches, 1.0);
motor GoalMotor = motor(PORT20, ratio18_1, false);
motor IntakePusher = motor(PORT19, ratio18_1, true);
motor IntakeArm = motor(PORT18, ratio36_1, true);

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