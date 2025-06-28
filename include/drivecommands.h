using namespace vex;

class DriveCommands {
public:

    DriveCommands()
    {

    }

    static void MotorRampVoltage();
    static void MotorRampRPM();
    static void MotorStep();
    static void MotorNoise();
    static void DriveFor();
    static bool CheckTurnsDone(PositionControl &leftPositionControl, PositionControl &rightPositionControl);

};
