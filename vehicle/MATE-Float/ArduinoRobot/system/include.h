#ifndef INCLUDE-SYSTEM_h
#define INCLUDE-SYSTEM_h

// Add libraries here
#include "libraries/REYAX/src/REYAX.h"
#include "libraries/RTC-1.7.1/src/I2C_RTC.h"
#include <Servo.h>

// Add definition files here
#include "definitions/constants.h"
#include "definitions/pins.h"
#include "definitions/calibration.h"
#include "definitions/profile.h"

// Add modules here in order of dependency
#include "modules/motor.h"
#include "modules/sensor.h"
#include "modules/button.h"

// Add packages here in order of dependency
#include "packages/control.h"

#endif
