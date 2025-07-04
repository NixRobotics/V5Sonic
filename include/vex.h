#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "v5.h"
#include "v5_vcs.h"

#include "robot-config.h"
#include "cheesydrive.h"
#include "simpledrive.h"
#include "robotlog.h"
#include "ui.h"
#include "ploter.h"
#include "MiniPID.h"
#include "speedcontrol.h"
#include "positioncontrol.h"
#include "drivecommands.h"

#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
