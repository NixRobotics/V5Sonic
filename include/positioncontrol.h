using namespace vex;

class PositionControl {
public:

    double cmdPosition = 0.0; // Desired position in revolutions
    double maxSpeed = 0.0; // Maximum speed in RPM
    double currentPosition = 0.0; // Current speed in RPM
    
    int timeoutTime = 0; // Timeout for stopping in ms, 0 means no timeout
    int timeoutStartTime = 0; // Start time for timeout in ms

    double settleThreshold = 2.0 / 600.0; // 600 ticks per revolution for blue cartridge, 0.00166 revs or 0.6deg
    int settleTime = 50; // default 50ms
    int settleStartTime = 0;

private:
    bool _isSettled = false; // Flag to indicate if the position is settled
    bool _isTimeout = false; // Flag to indicate if the position control has timed out
    bool _isRunning = false; // Flag to indicate if the position control is running

public:
    MiniPID positionPID; // PID controller for speed control

    // Constructor sets screen size and margin parameters.
    PositionControl()
    :
        cmdPosition(0.0),
        maxSpeed(1.0), // Normalized speed
        positionPID(6.0, 3.0 / 100.0, 0.0) // Initialize PID with P, I, D values
    {
        positionPID.setSetpoint(cmdPosition); // Initial setpoint
        positionPID.setOutputLimits(-maxSpeed, maxSpeed); // Output limits for the PID controller - we operate one sided
        // positionPID.setOutputRampRate(1.0); // ramp is take care of by speed controller
    }

    // Set the desired position in revolutions
    void setPosition(double position) {
        cmdPosition = position; // Set the desired speed
        positionPID.setSetpoint(cmdPosition); // Update the PID setpoint

        _isRunning = true; // Set the running flag
        _isSettled = false; // Reset the settled flag
        _isTimeout = false; // Reset the timeout flag
    }

    void setTimeout(int ms) {
        timeoutTime = ms; // Set the timeout for stopping
    }

    bool isSettled() const {
        return _isSettled; // Return the settled status
    }

    bool isTimedOut() const {
        return _isTimeout; // Return the timeout status
    }

    bool checkTimout() {
        if (timeoutStartTime != 0 && (vex::timer::system() - timeoutStartTime) >= timeoutTime) {
            _isTimeout = true; // Set the timeout flag
            OnEndCondition();
            return true; // Return true if timeout is reached
        }
        return false;
    }

    bool checkSettled() {

        double error = cmdPosition - currentPosition; // Calculate the error

        if (settleStartTime != 0) {
            if (fabs(error) <= settleThreshold) {
                if (vex::timer::system() - settleStartTime >= settleTime) {
                    _isSettled = true; // Set the settled flag if within threshold for the specified time
                    OnEndCondition();
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
    double getOutput(double actualPosition) {
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

    void OnEndCondition() {
        timeoutStartTime = 0; // Reset the timeout start time
        settleStartTime = 0; // Reset the settle start time
        _isRunning = false; // Reset the running flag
        positionPID.reset(); // Reset the PID controller
    }

    void reset() {
        positionPID.reset(); // Reset the PID controller
        settleStartTime = 0; // Reset the settle start time
        timeoutStartTime = 0;
        _isSettled = false; // Reset the settled flag
        _isTimeout = false; // Reset the timeout flag
        _isRunning = false; // Reset the running flag
    }

};
