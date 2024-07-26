using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern motor LeftFrontMotor;
extern motor RightFrontMotor;
extern motor LeftRearMotor;
extern motor RightRearMotor;
extern motor_group LeftDrive;
extern motor_group RightDrive;
extern drivetrain DriveTrain;
extern motor GoalMotor;
extern motor IntakePusher;
extern motor IntakeArm;

#define CONTROLLER_DEADBAND 5.0 // Joystick deadband for arcade drive

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);