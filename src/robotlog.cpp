#include "vex.h"

using namespace vex;

motor l1 = LeftFrontMotor;
motor l2 = LeftRearMotor;
motor r3 = RightFrontMotor;
motor r4 = RightRearMotor;

#define MAX_TORQUE (1.05 / 1.0)

bool bFirstPrint = true;
int printID = 0;
double lcmd = 0.0, rcmd = 0.0;
double lcmd_avg = 0.0, rcmd_avg = 0.0;
double l1trq_avg = 0.0, l2trq_avg = 0.0, r3trq_avg = 0.0, r4trq_avg = 0.0;
double l1spd_avg = 0.0, l2spd_avg = 0.0, r3spd_avg = 0.0, r4spd_avg = 0.0;
double l1tmp_avg = 0.0, l2tmp_avg = 0.0, r3tmp_avg = 0.0, r4tmp_avg = 0.0;

void printCallback()
{
    if (bFirstPrint) {
        printf("time, lcmd, rcmd, l1 trq, l2 trq, r3 trq, r4 trq, l1 spd, l2 spd, r3 spd, r4 spd, l1 tmp, l2 tmp, r3 tmp, r4 tmp\n");
        bFirstPrint = false;
    }

    printf("%4d, %3d, %3d, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf\n",
    printID, (int) lcmd_avg, (int) rcmd_avg, l1trq_avg, l2trq_avg, r3trq_avg, r4trq_avg, l1spd_avg, l2spd_avg, r3spd_avg, r4spd_avg, l1tmp_avg, l2tmp_avg, r3tmp_avg, r4tmp_avg);

}

double lcmd_accum = 0.0, rcmd_accum = 0.0;
double l1trq_accum = 0.0, l2trq_accum = 0.0, r3trq_accum = 0.0, r4trq_accum = 0.0;
double l1spd_accum = 0.0, l2spd_accum = 0.0, r3spd_accum = 0.0, r4spd_accum = 0.0;
double l1tmp_accum = 0.0, l2tmp_accum = 0.0, r3tmp_accum = 0.0, r4tmp_accum = 0.0;

void AccumStats()
{
    double l1trq = l1.torque(Nm);
    double l2trq = l2.torque(Nm);
    double r3trq = r3.torque(Nm);
    double r4trq = r4.torque(Nm);
    double l1spd = l1.velocity(percent);
    double l2spd = l2.velocity(percent);
    double r3spd = r3.velocity(percent);
    double r4spd = r4.velocity(percent);
    double l1tmp = l1.temperature(percent);
    double l2tmp = l2.temperature(percent);
    double r3tmp = r3.temperature(percent);
    double r4tmp = r4.temperature(percent);

    lcmd_accum += lcmd;
    rcmd_accum += rcmd;
    l1trq_accum += l1trq;
    l2trq_accum += l2trq;
    r3trq_accum += r3trq;
    r4trq_accum += r4trq;
    l1spd_accum += l1spd;
    l2spd_accum += l2spd;
    r3spd_accum += r3spd;
    r4spd_accum += r4spd;
    l1tmp_accum += l1tmp;
    l2tmp_accum += l2tmp;
    r3tmp_accum += r3tmp;
    r4tmp_accum += r4tmp;
}



void CalcStats()
{
    lcmd_avg = lcmd_accum / (double) STATSLOOPMAX;
    rcmd_avg = rcmd_accum / (double) STATSLOOPMAX;    
    l1trq_avg = 100.0 * (l1trq_accum / (double) STATSLOOPMAX) / MAX_TORQUE;
    l2trq_avg = 100.0 * (l2trq_accum / (double) STATSLOOPMAX) / MAX_TORQUE;
    r3trq_avg = 100.0 * (r3trq_accum / (double) STATSLOOPMAX) / MAX_TORQUE;
    r4trq_avg = 100.0 * (r4trq_accum / (double) STATSLOOPMAX) / MAX_TORQUE;
    l1spd_avg = l1spd_accum / (double) STATSLOOPMAX;
    l2spd_avg = l2spd_accum / (double) STATSLOOPMAX;
    r3spd_avg = r3spd_accum / (double) STATSLOOPMAX;
    r4spd_avg = r4spd_accum / (double) STATSLOOPMAX;
    l1tmp_avg = l1tmp_accum / (double) STATSLOOPMAX;
    l2tmp_avg = l2tmp_accum / (double) STATSLOOPMAX;
    r3tmp_avg = r3tmp_accum / (double) STATSLOOPMAX;
    r4tmp_avg = r4tmp_accum / (double) STATSLOOPMAX;

    lcmd_accum = 0.0, rcmd_accum = 0.0;
    l1trq_accum = 0.0, l2trq_accum = 0.0, r3trq_accum = 0.0, r4trq_accum = 0.0;
    l1spd_accum = 0.0, l2spd_accum = 0.0, r3spd_accum = 0.0, r4spd_accum = 0.0;
    l1tmp_accum = 0.0, l2tmp_accum = 0.0, r3tmp_accum = 0.0, r4tmp_accum = 0.0;
}

event printEvent = event();

void LogInit() {
    // Initializing Robot Configuration. DO NOT REMOVE!
    printEvent(printCallback);

}

void LogAccumState(double left_speed, double right_speed)
{
    lcmd = left_speed;
    rcmd = right_speed;

    AccumStats();
}

void LogCalcStats()
{
    CalcStats();

    printEvent.broadcast();
    printID++;
}
