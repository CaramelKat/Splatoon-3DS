/*
	Circle Pad example made by Aurelio Mannara for libctru
	Please refer to https://github.com/devkitPro/libctru/blob/master/libctru/include/3ds/services/hid.h for more information
	This code was modified for the last time on: 12/13/2014 2:20 UTC+1

	This wouldn't be possible without the amazing work done by:
	-Smealum
	-fincs
	-WinterMute
	-yellows8
	-plutoo
	-mtheall
	-Many others who worked on 3DS and I'm surely forgetting about
*/

#include <3ds.h>
#include <stdio.h>
#include <math.h>
#include "lib/gyro.h"

#define PI 3.14159265



int main(int argc, char **argv)
{
	//Matrix containing the name of each key. Useful for printing when a key is pressed
	char keysNames[32][32] = {
		"KEY_A", "KEY_B", "KEY_SELECT", "KEY_START",
		"KEY_DRIGHT", "KEY_DLEFT", "KEY_DUP", "KEY_DDOWN",
		"KEY_R", "KEY_L", "KEY_X", "KEY_Y",
		"", "", "KEY_ZL", "KEY_ZR",
		"", "", "", "",
		"KEY_TOUCH", "", "", "",
		"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
		"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
	};

	// Initialize services
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);

	u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0; //In these variables there will be information about keys detected in the previous frame

    HIDUSER_EnableGyroscope();

    //initialize the accelerometer
    initAcc();

	printf("\x1b[1;1HPress Start to exit.");

	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();
		//hidKeysHeld returns information about which buttons have are held down in this frame
		u32 kHeld = hidKeysHeld();
		//hidKeysUp returns information about which buttons have been just released
		u32 kUp = hidKeysUp();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		//Do the keys printing only if keys have changed
		if (kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld)
		{
			//Clear console
			consoleClear();

			//These two lines must be rewritten because we cleared the whole console
			printf("\x1b[1;1HPress Start to exit.");

			printf("\x1b[12;1H"); //Move the cursor to the fourth row because on the third one we'll write the circle pad position

			//Check if some of the keys are down, held or up
			int i;
			for (i = 0; i < 32; i++)
			{
				if (kDown & BIT(i)) printf("%s down\n", keysNames[i]);
				if (kHeld & BIT(i)) printf("%s held\n", keysNames[i]);
				if (kUp & BIT(i)) printf("%s up\n", keysNames[i]);
			}
		}

		//Set keys old values for the next frame
		kDownOld = kDown;
		kHeldOld = kHeld;
		kUpOld = kUp;

		circlePosition circlePos;

        angularRate gyroPos;

        accelVector accPos;

		//Read the CirclePad position
		hidCircleRead(&circlePos);

        hidGyroRead(&gyroPos);

        hidAccelRead(&accPos);

		//Print the CirclePad position
        printf("\x1b[3;1HCirclePad position:");
		printf("\x1b[4;1H%04d; %04d", circlePos.dx, circlePos.dy);

        if((kDown & KEY_Y)) {
            setHome();
        }
        printf("\x1b[6;1HGyro Data:");
        printf("\x1b[7;1HX: %04d Y: %04d Z: %04d", (gyroPos.x), (gyroPos.y), (gyroPos.z));

        accelVector positionVect = getPosition();
        printf("\x1b[9;1HAcc Data:");
        printf("\x1b[10;1HX: %04d Y: %04d Z: %04d", positionVect.x, positionVect.y, positionVect.z);

        /*if(accPos.y < -300) {
            printf("\x1b[12;1H   |   ");
            printf("\x1b[13;1H   V   ");
        }
        else if(accPos.y > 300) {
            printf("\x1b[12;1H   ^   ");
            printf("\x1b[13;1H   |   ");
        }
        else if(accPos.x > 300) {
            printf("\x1b[12;1H   <-");
            printf("\x1b[13;1H       ");
        }
        else if(accPos.x < -300) {
            printf("\x1b[12;1H   ->");
            printf("\x1b[13;1H       ");
        }*/

        /*float accelerationX = (signed int)(((signed int)accPos.x) * 3.9);
        float accelerationY = (signed int)(((signed int)accPos.y) * 3.9);
        float accelerationZ = (signed int)(((signed int)accPos.z) * 3.9);

        float pitch = 180 * atan (accelerationX/sqrt(accelerationY*accelerationY + accelerationZ*accelerationZ))/M_PI;
        float roll = 180 * atan (accelerationY/sqrt(accelerationX*accelerationX + accelerationZ*accelerationZ))/M_PI;
        float yaw = 180 * atan (accelerationZ/sqrt(accelerationX*accelerationX + accelerationZ*accelerationZ))/M_PI;*/

        /*if (gyroPos.y > 200 || gyroPos.y < -200) {
            yaw += gyroPos.y / 250;
        }
        if (gyroPos.x > 200 || gyroPos.x < -200) {
            pitch += gyroPos.x / 200;
        }*/
        //printf("\x1b[12;1HYaw: %1f Pitch: %1f Roll: %1f", roundf(yaw), roundf(pitch), roundf(roll));
        //printf("\x1b[13;1H");
        /*char line[48] = [" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
                          "x", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "];
        int j;
        for (j = 0; j < 48; j++)
        {
            printf("%s down\n", line[j]);
        }*/
        // printf("\x1b[13;1H|                        x                       |");


        printf("\x1b[13;1H|                        x                       |");


		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	// Exit services
	gfxExit();
	return 0;
}
