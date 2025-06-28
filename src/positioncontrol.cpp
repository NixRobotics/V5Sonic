#include "vex.h"

using namespace vex;

bool PositionControl::checkTimout() {
    if (timeoutStartTime != 0 && (vex::timer::system() - timeoutStartTime) >= timeoutTime) {
        _isTimeout = true; // Set the timeout flag
        _onEndCondition();
        return true; // Return true if timeout is reached
    }
    return false;
}

bool PositionControl::checkSettled() {

    double error = cmdPosition - currentPosition; // Calculate the error

    if (settleStartTime != 0) {
        if (fabs(error) <= settleThreshold) {
            if (vex::timer::system() - settleStartTime >= settleTime) {
                _isSettled = true; // Set the settled flag if within threshold for the specified time
                _onEndCondition();
                return true;
            }
        } else {
            settleStartTime = 0; // Reset the settle start time if error exceeds threshold
        }
    } else if (fabs(error) <= settleThreshold) {
        settleStartTime = vex::timer::system(); // Start the settle timer if within threshold
    }

    return false;
}

// Get the output of the PID controller in RPM based on the current position in revolutions
double PositionControl::getOutput(double actualPosition) {
    currentPosition = actualPosition; // Update the current speed

    double output = 0.0; // Initialize output

    // printf("%f %f\n", cmdPosition, currentPosition);
    if (_isRunning) {
        output = positionPID.getOutput(actualPosition); // Get the PID output based on the current speed

        // Check timeout
        if (checkTimout()) {
            output = 0.0; // Stop the motor if timeout is reached
        } else if (checkSettled()) {
            output = 0.0; // Stop the motor if settled
        }

    }

    return output; // Return the calculated (rpm) output
}

void PositionControl::_onEndCondition() {
    settleStartTime = 0; // Reset the settle start time
    timeoutStartTime = 0; // Reset the timeout start time
    _isRunning = false; // Reset the running flag
    positionPID.reset(); // Reset the PID controller
}

void PositionControl::reset() {
    _isSettled = false; // Reset the settled flag
    _isTimeout = false; // Reset the timeout flag
    _onEndCondition(); // Reset the position control state
}
