#include "vex.h"

using namespace vex;

// defined externally ...
// #define CONTROLLER_DEADBAND 5.0 // Joystick deadband for arcade drive

#define DRIVESLOW 50.0
#define DRIVEFAST 100.0
#define TURNSLOW 100.0
#define TURNFAST 100.0
float maxdrive = DRIVEFAST;
float maxturn = TURNFAST;

// Toggle the drive sensitivity
void ToggleDriveSpeed() {
  if (maxdrive == DRIVEFAST) {
    maxdrive = DRIVESLOW;
    maxturn = TURNSLOW;
  } else {
    maxdrive = DRIVEFAST;
    maxturn = TURNFAST;
  }
}

void SimpleDrive(double throttle, double turn)
{
    // Deadband stops the motors when Axis values are close to zero.
    float deadband = CONTROLLER_DEADBAND;

    float drivescale = maxdrive / (100.0 - deadband);
    float turnscale = maxturn / (100.0 - deadband);

    // controller deadband and throttle scaling
    if (fabs(throttle) < deadband) throttle = 0.0;
    else {
      throttle = (throttle > 0.0) ? throttle - deadband : throttle + deadband;
      throttle *= drivescale;
    }

    // reduce turn sensitiviy when robot is moving slowly
    float throttlemixLim = 50.0; // upper limit of throttle mixing (above this point, full turn allowed)
    // NOTE: Next 2 parameters should add up to 1.0 (throttlemixMinSens + throttlemixSlope = 1.0)
    float throttlemixMinSens = 0.35; // minimum turn sensitivity point (i.e. when turning in place)
    float throttlemixSlope = 0.65; // rate at which turn sensitivity increases with increased throttle

    if (fabs(throttle) < throttlemixLim) {
      float throttlemix = fabs(throttle) / throttlemixLim;
      turnscale = turnscale * (throttlemixMinSens + throttlemixSlope * throttlemix);
    }

    // controller deadband and turn scaling
    if (fabs(turn) < deadband) turn = 0.0;
    else {
      turn = (turn > 0.0) ? turn - deadband : turn + deadband;
      turn *= turnscale;
    }

    DriveTrain.arcade(throttle, turn);
}
