#include "vex.h"

using namespace vex;

void UIPrintInitial()
{
  printf("hello vex world\n");
#ifdef USING_6MOTORS
  Brain.Screen.print("## YOSHI-6 ##");
#else
  Brain.Screen.print("## YOSHI-4 ##");
#endif
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
}

void UIPrintCurrent(int dtmotor_temp[], int dtmotor_trqs[])
{
    int armtemp = (int) IntakeArm.temperature(pct);
    int pushtemp = (int) IntakePusher.temperature(pct);
    int goaltemp = (int) GoalMotor.temperature(pct);
    for (int i = 0; i < NUM_DTMOTORS; i++) {
        dtmotor_temp[i] = dtmotors[i].temperature(pct);
    }

    int driveTemp = 0;
    for (int i = 0; i < NUM_DTMOTORS; i++) {
        if (dtmotor_temp[i] > 70) driveTemp = 2;
        else if (dtmotor_temp[i] > 50) driveTemp = 1;
    }

    int armTemp = 0;
    if (armtemp >= 70 || pushtemp > 70 || goaltemp > 70) armTemp = 2;
    else if (armtemp > 50 || pushtemp > 50 || goaltemp > 50) armTemp = 1;

    if (driveTemp == 2 || armTemp == 2) {
        Brain.Screen.clearScreen(red);
        Brain.Screen.setFillColor(red);
    } else if (driveTemp == 1 || armTemp == 1) {
        Brain.Screen.clearScreen(orange);
        Brain.Screen.setFillColor(orange);
    } else {
        Brain.Screen.clearScreen(green);
        Brain.Screen.setFillColor(green);
    }
    Brain.Screen.setPenColor(black);
    Brain.Screen.setCursor(1, 1);
#ifdef USING_6MOTORS
    Brain.Screen.print("## YOSHI-6 ##");
#else
    Brain.Screen.print("## YOSHI-4 ##");
#endif
    Brain.Screen.setCursor(3, 1);
    Brain.Screen.print("Arm Temp: %3d", armtemp);
    Brain.Screen.setCursor(4, 1);
    Brain.Screen.print("Pusher Temp: %3d", pushtemp);
    Brain.Screen.setCursor(5, 1);
    Brain.Screen.print("Goal Temp: %3d", goaltemp);
    Brain.Screen.setCursor(6, 1);
#ifdef USING_6MOTORS
    Brain.Screen.print("Drive Temps: %3d %3d %3d %3d %3d %3d",
        dtmotor_temp[0],
        dtmotor_temp[1],
        dtmotor_temp[2],
        dtmotor_temp[3],
        dtmotor_temp[4],
        dtmotor_temp[5]
    );
#else
    Brain.Screen.print("Drive Temps: %3d %3d %3d %3d",
        dtmotor_temp[0],
        dtmotor_temp[1],
        dtmotor_temp[2],
        dtmotor_temp[3]
    );
#endif
    Brain.Screen.setCursor(7, 1);
#ifdef USING_6MOTORS
    Brain.Screen.print("Drive Torxs: %3d %3d %3d %3d %3d %3d",
        dtmotor_trqs[0],
        dtmotor_trqs[1],
        dtmotor_trqs[2],
        dtmotor_trqs[3],
        dtmotor_trqs[4],
        dtmotor_trqs[5]
    );
#else
    Brain.Screen.print("Drive Torxs: %3d %3d %3d %3d",
        dtmotor_trqs[0],
        dtmotor_trqs[1],
        dtmotor_trqs[2],
        dtmotor_trqs[3]
    );
#endif

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
