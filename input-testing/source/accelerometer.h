/*
 * Basic Library to make working with the accelerometer far easier
 * on the 3DS
 *
 * Written By: Jemma Poffinbarger
 * Last Updated: September 14th, 2022
 */
#pragma once
#include <3ds.h>

int precision = 10;
accelVector homePos;
accelVector accPos;
/**
 * Sets the home position of the accelerometer to the current position
 */
void setHome() {
    homePos.x = accPos.x;
    homePos.y = accPos.y;
    homePos.z = accPos.z;
}

/**
 * Returns the current position of the 3DS to the level of precision
 * defined in the precision variable
 * @return accelVector
 */
accelVector getPosition() {
    hidAccelRead(&accPos);
    accelVector temp;
    temp.x = ((homePos.x - accPos.x) / precision);
    temp.y = ((homePos.y  - accPos.y) / precision);
    temp.z = ((homePos.z  - accPos.z) / precision);
    return temp;
}

/**
 * Updates the precision of the accelerometer
 * @param num
 */
void setPrecision(int num) { precision = num; }

/**
 * Initializes the accelerometer and sets the home position on startup
 */
void initAcc() {
    HIDUSER_EnableAccelerometer();
    hidAccelRead(&accPos);
    setHome();
}
