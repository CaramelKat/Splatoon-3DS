/*
 * Wrapper library for communication via UDS
 * on the 3DS
 *
 * Written By: Jemma Poffinbarger
 * Last Updated: January 19th, 2023
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <vector>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <3ds.h>

#include "lib/network.h"

PrintConsole topScreen, bottomScreen;
Network::UDS UDSNet;

static SwkbdState swkbd;
static char keyboardBuf[60];
static SwkbdStatusData swkbdStatus;
static SwkbdLearningData swkbdLearning;
SwkbdButton button = SWKBD_BUTTON_NONE;

/**
 * DEBUG
 * Prints found networks
 * @param index
 */
void printNetworksMenu(size_t index) {
    size_t total_networks = UDSNet.getNumberOfNetworks();
    consoleSelect(&bottomScreen);
    iprintf("\x1b[2J");
    printf("%zx Available Networks:\n\n", total_networks);
    for(size_t i = 0; i < total_networks; i++) {
        printf("  ====================================\n\n");
        if(i == index) printf("> ");
        else printf("| ");
        std::vector<char> appdata = UDSNet.getNetworkAppData(i);
        std::vector<char> name = UDSNet.getNetworkOwnerUsername(i);
        printf("%.*s", 12, appdata.data());
        printf("\t\t%s", name.data());
        printf("|\n\n");
    }
    printf("  ====================================\n");
    consoleSelect(&topScreen);
}

/**
 * DEBUG
 * Runs a menu to select a network
 */
void showNetworkMenu() {
    size_t total_networks = UDSNet.getNumberOfNetworks();
    if(!total_networks)
        return;
    int index = 0;
    printNetworksMenu(index);
    while(1) {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();

        if(kDown & KEY_UP) {
            index--;
        }
        else if(kDown & KEY_DOWN) {
            index++;
        }
        if(index < 0)
            index = 0;
        if(index > (int)total_networks - 1)
            index = (int)total_networks - 1;
        if(kDown & KEY_A) {
            consoleSelect(&bottomScreen);
            iprintf("\x1b[2J");
            consoleSelect(&topScreen);
            UDSNet.connectToNetwork(index, false);
            break;
        }
        else if(kDown & KEY_B) {
            consoleSelect(&bottomScreen);
            iprintf("\x1b[2J");
            consoleSelect(&topScreen);
            UDSNet.connectToNetwork(index, true);
            break;
        }
        if(kDown & KEY_START) {
            return;
        }
        if(kDown) {
            printNetworksMenu(index);
        }
    }
}

/**
 * DEBUG
 * Runs the main menu to either create or search for a network
 */
void showMainMenu() {
    printf("Start network: A\nStart Search: B\nExit: Start\n");
    while(1) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if(kDown & KEY_A) {
            UDSNet.createNetwork();
            break;
        }
        if(kDown & KEY_B) {
            UDSNet.searchForNetworks(5);
            showNetworkMenu();
            break;
        }
        if(kDown & KEY_START) {
            return;
        }
    }
}

void keyboardInput() {
    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 3, -1);
    swkbdSetInitialText(&swkbd, keyboardBuf);
    swkbdSetHintText(&swkbd, "Message to send");
    swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&swkbd, SWKBD_BUTTON_MIDDLE, "~Middle~", true);
    swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Send", true);
    swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT);
    static bool reload = false;
    swkbdSetStatusData(&swkbd, &swkbdStatus, reload, true);
    swkbdSetLearningData(&swkbd, &swkbdLearning, reload, true);
    reload = true;
    button = swkbdInputText(&swkbd, keyboardBuf, sizeof(keyboardBuf));

    if (button != SWKBD_BUTTON_NONE)
    {
        printf("\n[You]: %s\n", keyboardBuf);
        if(button == 2)
            UDSNet.sendPacket(keyboardBuf, strlen((char *)keyboardBuf) + 1, NULL);
    } else
        printf("swkbd event: %d\n", swkbdGetResult(&swkbd));
}

void uds_test()
{
    showMainMenu();

    printf("Press A to stop data transfer.\n");
    u32 transfer_data = 0, prev_transfer_data = 0;
    while(1)
    {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();

        if(kDown & KEY_A)break;

        if(kDown & KEY_B) {
            keyboardInput();
            continue;
        }
        prev_transfer_data = transfer_data;
        transfer_data = hidKeysHeld();

        //When the output from hidKeysHeld() changes, send it over the network.
        if(transfer_data != prev_transfer_data)//Spectators aren't allowed to send data.
        {
            char *data = "Testing data\0";
            #pragma GCC diagnostic ignored "-Wconversion-null"
            UDSNet.sendPacket(data, strlen((char *)data) + 1, NULL);
        }
        // pull data
        if(UDSNet.packetAvailable(false, false))//Check whether data is available via udsPullPacket().
        {
            u16 src_NetworkNodeID = 0;
            std::vector<uint8_t> buffer = UDSNet.pullPacket(src_NetworkNodeID);
            std::vector<char> username = UDSNet.getNodeUsername(src_NetworkNodeID);
            printf("\n[%s]: %s\n", (char *)username.data(), (char *)buffer.data());
        }

        if(UDSNet.connectionStatusAvailable(false, false))
        {
            int status = UDSNet.parseStatus(UDSNet.getConnStatus());
            switch(status) {
                case -1:
                    printf("[Connection Status] Unknown 0x6 State\n");
                    break;
                case 1:
                    printf("[Connection Status] Network Created\n");
                    break;
                case 2:
                    printf("[Connection Status] Joined Network\n");
                    break;
                case 3:
                    printf("[Connection Status] Client Connected\n");
                    break;
                case 4:
                    printf("[Connection Status] Client Disconnected\n");
                    break;
                case 5:
                    printf("[Connection Status] Host Terminated Network\n");
                    break;
            }
        }
    }

    UDSNet.terminateNetwork();
}

int main()
{
    gfxInitDefault();

    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    consoleSelect(&topScreen);

    printf("libctru UDS local-WLAN demo.\n");

    UDSNet.initUDS();
    uds_test();

    printf("Press START to exit.\n");

    // Main loop
    while (aptMainLoop())
    {
        gspWaitForVBlank();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }

    gfxExit();
    return 0;
}