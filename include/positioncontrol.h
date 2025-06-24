using namespace vex;

class PositionControl {
public:
    double cmdPosition = 0.0; // Desired position in revolutions
    double maxSpeed = 0.0; // Maximum speed in RPM
    double currentPosition = 0.0; // Current speed in RPM
    MiniPID positionPID; // PID controller for speed control
    // Constructor sets screen size and margin parameters.
    PositionControl()
    :
        cmdPosition(0.0),
        maxSpeed(200.0), // Set maximum speed to 200 RPM
        positionPID(0.01, 0.0, 0.0) // Initialize PID with P, I, D values
    {
        positionPID.setSetpoint(cmdPosition); // Initial setpoint
        positionPID.setOutputLimits(-maxSpeed, maxSpeed); // Output limits for the PID controller - we operate one sided
        // positionPID.setOutputRampRate(1.0); // ramp is take care of by speed controller
    }

    void setPosition(int position) {
        cmdPosition = position; // Set the desired speed
        positionPID.setSetpoint(cmdPosition); // Update the PID setpoint
    }

    double getOutput(double actualPosition) {
        currentPosition = actualPosition; // Update the current speed
        double output = positionPID.getOutput(actualPosition); // Get the PID output based on the current speed

        return output; // Return the calculated output
    }

};
