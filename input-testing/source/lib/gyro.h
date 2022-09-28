/*
 * Basic Library to make working with the gyroscope far easier
 * on the 3DS
 *
 * Written By: Jemma Poffinbarger
 * Last Updated: September 14th, 2022
 */
#pragma once
#include <3ds.h>
#include <math.h>

#define DRIFT_CORRECTION 12 // Range of inputs to ignore from the gyro before doing a calculation
#define dt 0.01 // 10 ms sample rate!

float precision = 10.0, roll = 0.0, pitch = 0.0, oldX = 0.0, oldY = 0.0;
angularRate gyroPos;

/**
 * Updates the pitch and roll values
 */
void update() {
    hidGyroRead(&gyroPos);
    // if the current X is between the old X +- DRIFT_CORRECTION, ignore the input
    if(!(ceil(gyroPos.x) >= ceil(oldX) - DRIFT_CORRECTION && ceil(gyroPos.x) <= ceil(oldX) + DRIFT_CORRECTION)) {
        pitch += ((float)gyroPos.x / precision) * dt; // Angle around the X-axis
    }
    // if the current X is between the old X +- DRIFT_CORRECTION, ignore the input
    if(!(ceil(gyroPos.y) >= ceil(oldY) - DRIFT_CORRECTION && ceil(gyroPos.y) <= ceil(oldY) + DRIFT_CORRECTION)) {
        roll -= ((float)gyroPos.y / precision) * dt;    // Angle around the Y-axis
    }
    oldX = gyroPos.x;
    oldY = gyroPos.y;
}

/**
 * Sets the home position of the accelerometer to the current position
 */
void setHome() {
    roll = 0.0;
    pitch = 0.0;
}

/**
 * Returns the current position of the 3DS to the level of precision
 * defined in the precision variable
 * @return Position
 */
void getPosition(float *tempPitch, float *tempRoll) {
    update();
    *tempPitch = ceil(pitch);
    *tempRoll = ceil(roll);
}

/**
 * Updates the precision of the accelerometer
 * @param num
 */
void setPrecision(float num) { precision = num; }

/**
 * Initializes the accelerometer and sets the home position on startup
 */
void init() {
    HIDUSER_EnableAccelerometer();
    HIDUSER_EnableGyroscope();
    update();
    setHome();
}
