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

// TODO: Remove for debugging
PrintConsole topScreen, bottomScreen;
Network::UDS UDSNet;

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
        char *tmp = new char[0x14];
        char *name = new char[0x0b];
        UDSNet.getNetworkAppData(i, tmp);
        UDSNet.getNetworkOwnerUsername(i, name);
        printf("%.*s", 10, tmp);
        printf("\t%s", name);
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

void uds_test()
{
    if(UDSNet.connectionStatusAvailable(false, false))
    {
        printf("Constatus event signaled.\n");
    }

    showMainMenu();

    printf("Press A to stop data transfer.\n");
    u32 transfer_data, prev_transfer_data = 0;
    while(1)
    {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();

        if(kDown & KEY_A)break;
        prev_transfer_data = transfer_data;
        transfer_data = hidKeysHeld();

        //When the output from hidKeysHeld() changes, send it over the network.
        if(transfer_data != prev_transfer_data)//Spectators aren't allowed to send data.
        {
            char *data = "Testing data\0";
            UDSNet.sendPacket(data, strlen((char *)data) + 1, NULL);
        }
        // pull data
        if(UDSNet.packetAvailable(false, false))//Check whether data is available via udsPullPacket().
        {
            u16 src_NetworkNodeID = 0;
            std::vector<uint8_t> buffer = UDSNet.pullPacket(src_NetworkNodeID);
            printf("\t\"%s\"\n", (char *)buffer.data());
            printf("%d\n\n", src_NetworkNodeID);
        }

        if(UDSNet.connectionStatusAvailable(false, false))
        {
            printf("\n\nConstatus event signaled.\n");
            udsConnectionStatus constatus = UDSNet.getConnStatus();
            printf("status=0x%x\n", (unsigned int)constatus.status);
            printf("node_bitmask=0x%x\n", (unsigned int)constatus.total_nodes);
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