using namespace vex;

class SpeedControl {
public:
    int cmdSpeed = 0; // Desired speed in RPM
    int currentSpeed = 0; // Current speed in RPM
    double inputMedianFilter[3] = {0.0, 0.0, 0.0}; // Median filter for actual speed smoothing
    MiniPID speedPID; // PID controller for speed control
    // Constructor sets screen size and margin parameters.
    SpeedControl()
    : cmdSpeed(0),
      speedPID(0.005, 0.0005, 0.0) // Initialize PID with P, I, D values
    {
        speedPID.setF(1.0 / 61.25); // Feedforward for speed control
        speedPID.setFOffset(18.4); // Offset for feedforward
        speedPID.setSetpoint((double) cmdSpeed); // Initial setpoint
        speedPID.setOutputLimits(-12.0, 12.0); // Output limits for the PID controller - we operate one sided
        speedPID.setOutputRampRate(8.0 / 10.0); // Set output ramp rate to 8V per 100ms (or 0.4V per 10ms)
    }

    void setSpeed(int speed) {
        cmdSpeed = speed; // Set the desired speed
        speedPID.setSetpoint((double) cmdSpeed); // Update the PID setpoint
    }

    double getMedianSpeed(double actualSpeed) {
        // Calculate the median of the last three speed readings
        inputMedianFilter[2] = inputMedianFilter[1];
        inputMedianFilter[1] = inputMedianFilter[0];
        inputMedianFilter[0] = actualSpeed;

        // Sort the array to find the median
        double sorted[3] = {inputMedianFilter[0], inputMedianFilter[1], inputMedianFilter[2]};
        std::sort(sorted, sorted + 3);

        // printf("%0.2f, %0.2f, %0.2f, %0.2f\n", 
        //        inputMedianFilter[0], inputMedianFilter[1], inputMedianFilter[2], sorted[1]);
        return sorted[1]; // Return the middle value as the median
    }

    double getOutput(double actualSpeed) {
        currentSpeed = actualSpeed; // Update the current speed
        double medianSpeed = getMedianSpeed((double) actualSpeed); // Get the median speed
        double output = speedPID.getOutput(medianSpeed); // Get the PID output based on the current speed

        // sanity checks
        if (cmdSpeed == 0) output = 0.0; // If the command speed is zero, set output to zero
        if (output < -12.0) output = -12.0; // Ensure output is non-negative, as we operate one sided
        if (output > 12.0) output = 12.0; // Cap the output at 12V

        return output; // Return the calculated output
    }

};
