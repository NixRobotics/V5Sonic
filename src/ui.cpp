#include "vex.h"

using namespace vex;

void UIPrintInitial()
{
  printf("hello vex world\n");
#ifdef USING_6MOTORS
  Brain.Screen.print("## SONIC-6 ##");
#else
  Brain.Screen.print("## SONIC-4 ##");
#endif
  Brain.Screen.newLine();

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
}

void UIPrintCurrent(int dtmotor_temp[], int dtmotor_trqs[])
{
    for (int i = 0; i < NUM_DTMOTORS; i++) {
        dtmotor_temp[i] = dtmotors[i].temperature(pct);
    }

    int driveTemp = 0;
    for (int i = 0; i < NUM_DTMOTORS; i++) {
        if (dtmotor_temp[i] > 70) driveTemp = 2;
        else if (dtmotor_temp[i] > 50) driveTemp = 1;
    }


    if (driveTemp == 2) {
        Brain.Screen.clearScreen(red);
        Brain.Screen.setFillColor(red);
    } else if (driveTemp == 1) {
        Brain.Screen.clearScreen(orange);
        Brain.Screen.setFillColor(orange);
    } else {
        Brain.Screen.clearScreen(green);
        Brain.Screen.setFillColor(green);
    }
    Brain.Screen.setPenColor(black);
    Brain.Screen.setCursor(1, 1);
#ifdef USING_6MOTORS
    Brain.Screen.print("## SONIC-6 ##");
#else
    Brain.Screen.print("## SONIC-4 ##");
#endif
    Brain.Screen.setCursor(3, 1);
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
}
