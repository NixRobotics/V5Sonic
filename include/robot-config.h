using namespace vex;

extern brain Brain;

// VEXcode devices
#define USING_6MOTORS

extern controller Controller1;
extern motor L1, L2, L3;
extern motor R1, R2, R3;
extern motor_group LeftDrive;
extern motor_group RightDrive;
extern drivetrain DriveTrain;
extern motor GoalMotor;
extern motor IntakePusher;
extern motor IntakeArm;
extern motor ConveyorMotor;
extern motor dtmotors[];

#ifdef USING_6MOTORS
#define NUM_DTMOTORS    6
#else
#define NUM_DTMOTORS    4
#endif

#define CONTROLLER_DEADBAND 5.0 // Joystick deadband for arcade drive

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);