#include "vex.h"
#include <string>
#include <iostream>

using namespace vex;

#define MAX_TORQUE (1.05 / 3.0)

bool bFirstPrint = true;
int printID = 0;
double lcmd = 0.0, rcmd = 0.0;
double lcmd_avg = 0.0, rcmd_avg = 0.0;
double trq[NUM_DTMOTORS], spd[NUM_DTMOTORS], tmp[NUM_DTMOTORS];
double trq_avg[NUM_DTMOTORS], trq_accum[NUM_DTMOTORS];
double spd_avg[NUM_DTMOTORS], spd_accum[NUM_DTMOTORS];
double tmp_avg[NUM_DTMOTORS], tmp_accum[NUM_DTMOTORS];

bool LOGGING_ENABLED = false;

void printCallback()
{
    if (!LOGGING_ENABLED) return;

    if (bFirstPrint) {
#ifdef USING_6MOTORS
        printf("time, lcmd, rcmd, l1 trq, l2 trq, l3 trq, r4 trq, r5 trq, r6 trq, l1 spd, l2 spd, l3 spd, r4 spd, r5 spd, r6 spd, l1 tmp, l2 tmp, l3 tmp, r4 tmp, r5 tmp, r6 tmp\n");
#else
        printf("time, lcmd, rcmd, l1 trq, l2 trq, r3 trq, r4 trq, l1 spd, l2 spd, r3 spd, r4 spd, l1 tmp, l2 tmp, r3 tmp, r4 tmp\n");
#endif
        bFirstPrint = false;
    }

    printf("%4d, %3d, %3d",
    printID, (int) lcmd_avg, (int) rcmd_avg);
    for (int i = 0; i < NUM_DTMOTORS; i++) printf(", %6.2lf", trq_avg[i]);
    for (int i = 0; i < NUM_DTMOTORS; i++) printf(", %6.2lf", spd_avg[i]);
    for (int i = 0; i < NUM_DTMOTORS; i++) printf(", %6.2lf", tmp_avg[i]);
    printf("\n");


}

double lcmd_accum = 0.0, rcmd_accum = 0.0;

void AccumStats()
{
    if (!LOGGING_ENABLED) return;

    for (int i = 0; i < NUM_DTMOTORS; i++) {
        trq[i] = dtmotors[i].torque(Nm);
        spd[i] = dtmotors[i].velocity(percent);
        tmp[i] = dtmotors[i].temperature(percent);
    }

    lcmd_accum += lcmd;
    rcmd_accum += rcmd;
    for (int i = 0; i < NUM_DTMOTORS; i++) {
        trq_accum[i] += trq[i];
        spd_accum[i] += spd[i];
        tmp_accum[i] += tmp[i];
    }

}

void CalcStats()
{
    if (!LOGGING_ENABLED) return;

    lcmd_avg = lcmd_accum / (double) STATSLOOPMAX;
    rcmd_avg = rcmd_accum / (double) STATSLOOPMAX;
    for (int i = 0; i < NUM_DTMOTORS; i++) {
        trq_avg[i] = 100.0 * (trq_accum[i] / (double) STATSLOOPMAX) / MAX_TORQUE;
        spd_avg[i] = spd_accum[i] / (double) STATSLOOPMAX;
        tmp_avg[i] = tmp_accum[i] / (double) STATSLOOPMAX;
    }

    lcmd_accum = 0.0, rcmd_accum = 0.0;
    for (int i = 0; i < NUM_DTMOTORS; i++) {
        trq_accum[i] = 0.0;
        spd_accum[i] = 0.0;
        tmp_accum[i] = 0.0;
    }
}

event printEvent = event();

void LogInit() {
    if (!LOGGING_ENABLED) return;

    // Initializing Robot Configuration. DO NOT REMOVE!
    printEvent(printCallback);

}

void LogAccumState(double left_speed, double right_speed)
{
    if (!LOGGING_ENABLED) return;

    lcmd = left_speed;
    rcmd = right_speed;

    AccumStats();
}

void LogCalcStats()
{
    if (!LOGGING_ENABLED) return;

    CalcStats();

    printEvent.broadcast();
    printID++;
}

struct tSlingLog {
    int32_t time;
    float pos;
    float torque;
};

#define MAXLOG 20000
tSlingLog LOGARRAY[MAXLOG];
int32_t logId = 0;

void slingChanged(float pos, float torque)
{
    if (logId < MAXLOG) {
        LOGARRAY[logId].time = vex::timer::system();
        LOGARRAY[logId].pos = pos;
        LOGARRAY[logId].torque = torque;
        logId++;
    } else if (logId == MAXLOG) {
        printf("log buffer overflow\n");
        logId++;
    }
}

void DumpLog()
{
    if (!Brain.SDcard.isInserted()) {
        printf("No SDCARD\n");
    }
    if (logId >= MAXLOG) logId = MAXLOG;
    std::string str = "";
    char tempStr[100];
    str += "time, deg, torque\n";
    for (int i = 0; i < logId; i++) {
        sprintf(&(tempStr[0]), "%ld, %f, %f\n", LOGARRAY[i].time, LOGARRAY[i].pos, LOGARRAY[i].torque);
        str += tempStr;
        // str += LOGARRAY[i].time + LOGARRAY[i].val + "\n";
    }

    const char *cStr = str.c_str();
    int len = strlen(cStr);

    if (Brain.SDcard.isInserted()) {
        int saved = Brain.SDcard.savefile("data.csv", (uint8_t *) cStr, len);

        printf("%d of %d bytes written to file\n", saved, len);
        if (Brain.SDcard.exists("data.csv")) {
            printf("File size: %ld\n", Brain.SDcard.size("data.csv"));        }
    } else {
        printf("%s", cStr);
    }
}

