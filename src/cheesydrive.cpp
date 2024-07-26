#include "vex.h"

using namespace vex;

/**
 * Cheesy Drive Constants
 */
#define DRIVE_DEADBAND 0.1f
#define DRIVE_SLEW 0.02f
#define CD_TURN_NONLINEARITY                                                   \
  0.65 // This factor determines how fast the wheel
       // traverses the "non linear" sine curve
#define CD_NEG_INERTIA_SCALAR 4.0
#define CD_SENSITIVITY 1.0

// We apply a sinusoidal curve (twice) to the joystick input to give finer
// control at small inputs.
static double _turnRemapping(double iturn) {
	double denominator = sin(M_PI / 2 * CD_TURN_NONLINEARITY);
	double firstRemapIteration =
	    sin(M_PI / 2 * CD_TURN_NONLINEARITY * iturn) / denominator;
	return sin(M_PI / 2 * CD_TURN_NONLINEARITY * firstRemapIteration) / denominator;
}

// On each iteration of the drive controller (where we aren't point turning) we
// constrain the accumulators to the range [-1, 1].
double quickStopAccumlator = 0.0;
double negInertiaAccumlator = 0.0;
static void _updateAccumulators() {
	if (negInertiaAccumlator > 1) {
		negInertiaAccumlator -= 1;
	} else if (negInertiaAccumlator < -1) {
		negInertiaAccumlator += 1;
	} else {
		negInertiaAccumlator = 0;
	}

	if (quickStopAccumlator > 1) {
		quickStopAccumlator -= 1;
	} else if (quickStopAccumlator < -1) {
		quickStopAccumlator += 1;
	} else {
		quickStopAccumlator = 0.0;
	}
}

double prevTurn = 0.0;
double prevThrottle = 0.0;
void cheesyDrive(double ithrottle, double iturn) {
    ithrottle = ithrottle / 100.0;
    iturn = iturn / 100.0;

	bool turnInPlace = false;
	double linearCmd = ithrottle;
	if (fabs(ithrottle) < DRIVE_DEADBAND && fabs(iturn) > DRIVE_DEADBAND) {
		// The controller joysticks can output values near zero when they are
		// not actually pressed. In the case of small inputs like this, we
		// override the throttle value to 0.
		linearCmd = 0.0;
		turnInPlace = true;
	} else if (ithrottle - prevThrottle > DRIVE_SLEW) {
		linearCmd = prevThrottle + DRIVE_SLEW;
	} else if (ithrottle - prevThrottle < -(DRIVE_SLEW * 2)) {
		// We double the drive slew rate for the reverse direction to get
		// faster stopping.
		linearCmd = prevThrottle - (DRIVE_SLEW * 2);
	}

	double remappedTurn = _turnRemapping(iturn);

	double left, right;
	if (turnInPlace) {
		// The remappedTurn value is squared when turning in place. This
		// provides even more fine control over small speed values.
		left = remappedTurn * fabs(remappedTurn);
		right = -remappedTurn * fabs(remappedTurn);

		// The FRC Cheesy Drive Implementation calculated the
		// quickStopAccumulator here:
		// if (Math.abs(linearPower) < 0.2) {
		// 	double alpha = 0.1;
		// 	quickStopAccumulator = (1 - alpha) * quickStopAccumulator
		// 			+ alpha * Util.limit(wheel, 1.0) * 5;
		// }
		// But I apparently left that out of my implementation? Seemed to work
		// without it though.
	} else {
		double negInertiaPower = (iturn - prevTurn) * CD_NEG_INERTIA_SCALAR;
		negInertiaAccumlator += negInertiaPower;

		double angularCmd =
		    fabs(linearCmd) *  // the more linear vel, the faster we turn
		        (remappedTurn + negInertiaAccumlator) *
		        CD_SENSITIVITY -  // we can scale down the turning amount by a
		                          // constant
		    quickStopAccumlator;

		right = left = linearCmd;
		left += angularCmd;
		right -= angularCmd;

		_updateAccumulators();
	}

	// DriveTrain.arcade(updown, leftright);
    // controller->tank(left, right);
    LeftDrive.spin(fwd, left * 100.0, pct);
    RightDrive.spin(fwd, right * 100.0, pct);

	prevTurn = iturn;
	prevThrottle = ithrottle;
}
