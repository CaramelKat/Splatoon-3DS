/*
 * Wrapper library for communication via UDS
 * on the 3DS
 *
 * Written By: Jemma Poffinbarger
 * Last Updated: December 19th, 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <3ds.h>

const u32 receive_buffer_size = UDS_DEFAULT_RECVBUFSIZE;
const u32 wlancommID = 0x48424200;
const u8 data_channel = 1;
const char AppDataString[0x11] = "Ur a kid ur a sq";
const char *passphrase = "You're a squid now! You're a kid now!";

_Static_assert(18 > sizeof(AppDataString), "AppDataString too long! Must be less than 16 chars.");

u8 appdata[0x14] = {0x69, 0x8a, 0x05, 0x5c};

// 0 host 1 client
u32 con_type =  0;

udsBindContext bindctx;
udsNetworkScanInfo *networks = NULL;
udsNetworkScanInfo *network = NULL;
udsConnectionType conntype = UDSCONTYPE_Client;

size_t total_networks = 0;

u32 transfer_data, prev_transfer_data = 0;
size_t actual_size;
u16 src_NetworkNodeID;

// TODO: Fix this. This is awful. Need to find better way to pass data around
udsNodeInfo tmpnode;
char tmpstr[256];
u8 out_appdata[0x14];

// TODO: Remove for debugging
PrintConsole topScreen, bottomScreen;

/**
 * Initialize the UDS Library
 */
Result initUDS() {
    Result ret = 0;
    ret = udsInit(0x3000, NULL); //The sharedmem size only needs to be slightly larger than the total receive_buffer_size for all binds, with page-alignment.
    if(R_FAILED(ret))
        printf("udsInit failed: 0x%08x.\n", (unsigned int)ret);
    return ret;
}

/**
 * Searches for nearby networks that can be connected to
 * @param iterations
 * @return Number of networks found
 */
size_t searchForNetworks(int iterations) {
    u32 *tmpbuf;
    size_t tmpbuf_size;
    size_t network_count = 0;

    tmpbuf_size = 0x4000;
    tmpbuf = (u32*)malloc(tmpbuf_size);
    if(tmpbuf==NULL)
    {
        printf("Failed to allocate tmpbuf for beacon data.\n");
        return network_count;
    }

    //With normal client-side handling you'd keep running network-scanning until the user chooses to stops scanning or selects a network to connect to. This example just scans a maximum of 10 times until at least one network is found.
    for(int i = 0; i < iterations; i++)
    {
        memset(tmpbuf, 0, sizeof(tmpbuf_size));
        udsScanBeacons(tmpbuf, tmpbuf_size, &networks, &network_count, wlancommID, 0, NULL, false);
        printf("total_networks=%u.\n", (unsigned int)network_count);
    }

    free(tmpbuf);
    tmpbuf = NULL;

    return network_count;
}

/**
 * Gets the appdata for the specified network
 * @param index
 * @return char* of appdata from network
 */
void getNetworkAppData(int index, u8 out_appdata) {
    Result ret = 0;
    udsNetworkScanInfo *tmpNetwork = &networks[index];
    actual_size = 0;

    ret = udsGetNetworkStructApplicationData(&tmpNetwork->network, out_appdata, sizeof(out_appdata), &actual_size);

    if(R_FAILED(ret) || actual_size!=sizeof(out_appdata))
    {
        printf("udsGetNetworkStructApplicationData() returned 0x%08x. actual_size = 0x%x.\n", (unsigned int)ret, actual_size);
        free(networks);
        return "Invalid AppData size";
    }
    if(memcmp(out_appdata, appdata, 4) != 0)
    {
        printf("The first 4-bytes of appdata is invalid.\n");
        free(networks);
        return "Invalid AppData token";
    }
    return (char*)&out_appdata[4];
}

/**
 * Connects to a network by it's index either as a client
 * or a spectator.
 * @param index
 * @param spectator
 */
Result connectToNetwork(int index, bool spectator) {
    //At this point you'd let the user select which network to connect to and optionally display the first node's username(the host),
    // along with the parsed appdata if you want. For this example this just uses the first detected network and then displays the username of each node.
    //If appdata isn't enough, you can do what DLP does loading the icon data etc: connect to the network as a spectator temporarily for receiving broadcasted data frames.
    Result ret = 0;
    if(!total_networks)
        return ret;
    network = &networks[index];

    printf("network: total nodes = %u.\n", (unsigned int)network->network.total_nodes);

    for(u32 i = 0; i < UDS_MAXNODES; i++)
    {
        if(!udsCheckNodeInfoInitialized(&network->nodes[i]))continue;

        memset(tmpstr, 0, sizeof(tmpstr));

        ret = udsGetNodeInfoUsername(&network->nodes[i], tmpstr);
        if(R_FAILED(ret))
        {
            printf("udsGetNodeInfoUsername() returned 0x%08x.\n", (unsigned int)ret);
            free(networks);
            return ret;
        }

        printf("node%u username: %s\n", (unsigned int)i, tmpstr);
    }

    if(spectator)
    {
        conntype = UDSCONTYPE_Spectator;
        printf("Connecting to the network as a spectator...\n");
    }
    else
    {
        printf("Connecting to the network as a client...\n");
    }
    for(u32 i = 0; i < 10; i++)
    {
        ret = udsConnectNetwork(&network->network, passphrase, strlen(passphrase)+1, &bindctx, UDS_BROADCAST_NETWORKNODEID, conntype, data_channel, receive_buffer_size);
        if(R_FAILED(ret))
        {
            printf("udsConnectNetwork() returned 0x%08x.\n", (unsigned int)ret);
            return ret;
        }
        else
        {
            break;
        }
    }

    free(networks);
    con_type = 1;
    printf("Connected.\n");
    return ret;
}

/**
 * Creates a new network
 */
Result createNetwork() {
    Result ret = 0;
    udsNetworkStruct networkstruct;
    udsGenerateDefaultNetworkStruct(&networkstruct, wlancommID, 0, UDS_MAXNODES);
    strncpy((char*)&appdata[4], AppDataString, sizeof(appdata)-4);

    printf("Creating the network...\n");
    ret = udsCreateNetwork(&networkstruct, passphrase, strlen(passphrase)+1, &bindctx, data_channel, receive_buffer_size);
    if(R_FAILED(ret))
    {
        printf("udsCreateNetwork() returned 0x%08x.\n", (unsigned int)ret);
        return ret;
    }
    //If you want to use appdata, you can set the appdata whenever you want after creating the network.
    // If you need more space for appdata, you can set different chunks of appdata over time.
    ret = udsSetApplicationData(appdata, sizeof(appdata));
    if(R_FAILED(ret))
    {
        printf("udsSetApplicationData() returned 0x%08x.\n", (unsigned int)ret);
        udsDestroyNetwork();
        udsUnbind(&bindctx);
        return ret;
    }
    if(R_FAILED(ret))
    {
        udsDestroyNetwork();
        udsUnbind(&bindctx);
        return ret;
    }
    con_type = 0;
    return ret;
}

/**
 * Get a specified network's host username
 * @param index
 * @return
 */
char* getNetworkOwnerUsername(int index) {
    Result ret = 0;
    char tmpstr[256];
    if(!udsCheckNodeInfoInitialized(&networks[index].nodes[0]))
        return "Unknown";

    memset(tmpstr, 0, sizeof(tmpstr));

    ret = udsGetNodeInfoUsername(&networks[index].nodes[0], tmpstr);
    if(R_FAILED(ret))
    {
        printf("udsGetNodeInfoUsername() returned 0x%08x.\n", (unsigned int)ret);
        free(networks);
        return "Unknown";
    }

    return tmpstr;
}

/**
 * Prevent new connections
 * @param connections
 */
Result blockNewConnections(bool connections) {
    if(con_type == 1)
        return udsSetNewConnectionsBlocked(connections, true, false);
    return -3;
}

/**
 * Prevent spectators from joining network
 * @param spectators
 */
Result blockSpectators(bool spectators) {
    Result ret = 0;
    if(spectators)
        ret = udsEjectSpectator();
    else
        ret = udsAllowSpectators();
    return ret;
}

/**
 * Terminates the network. If host, it disconnects all users
 * if a client, disconnects from the network
 */
Result terminateNetwork() {
    Result ret = 0;
    if(con_type)
        ret = udsDestroyNetwork();
    else
        ret = udsDisconnectNetwork();
    udsUnbind(&bindctx);
    udsExit();
    return ret;
}

/**
 * DEBUG
 * Prints found networks
 * @param index
 */
void printNetworksMenu(size_t index) {
    consoleSelect(&bottomScreen);
    iprintf("\x1b[2J");
    printf("%x Available Networks:\n\n", total_networks);
    for(size_t i = 0; i < total_networks; i++) {
        printf("  ====================================\n\n");
        if(i == index) printf("> ");
        else printf("| ");
        printf("%.*s", 10, getNetworkAppData(i));
        printf("\t%s", getNetworkOwnerUsername(i));
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
            connectToNetwork(index, false);
            break;
        }
        else if(kDown & KEY_B) {
            consoleSelect(&bottomScreen);
            iprintf("\x1b[2J");
            consoleSelect(&topScreen);
            connectToNetwork(index, true);
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
            createNetwork();
            break;
        }
        if(kDown & KEY_B) {
            total_networks = searchForNetworks(5);
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
    u32 *tmpbuf;
    size_t tmpbuf_size;

    if(udsWaitConnectionStatusEvent(false, false))
    {
        printf("Constatus event signaled.\n");
    }

    showMainMenu();

    printf("Press A to stop data transfer.\n");

    tmpbuf_size = UDS_DATAFRAME_MAXSIZE;
    tmpbuf = (u32*)malloc(tmpbuf_size);
    if(tmpbuf == NULL)
    {
        printf("Failed to allocate tmpbuf for receiving data.\n");
        terminateNetwork();
        return;
    }
    Result ret = 0;
    while(1)
    {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();

        if(kDown & KEY_A)break;
        prev_transfer_data = transfer_data;
        transfer_data = hidKeysHeld();

        //When the output from hidKeysHeld() changes, send it over the network.
        if(transfer_data != prev_transfer_data && conntype!=UDSCONTYPE_Spectator)//Spectators aren't allowed to send data.
        {
            ret = udsSendTo(UDS_BROADCAST_NETWORKNODEID, data_channel, UDS_SENDFLAG_Default, &transfer_data, sizeof(transfer_data));
            if(UDS_CHECK_SENDTO_FATALERROR(ret))
            {
                printf("udsSendTo() returned 0x%08x.\n", (unsigned int)ret);
                break;
            }
        }
        // pull data
        if(udsWaitDataAvailable(&bindctx, false, false))//Check whether data is available via udsPullPacket().
        {
            memset(tmpbuf, 0, tmpbuf_size);
            actual_size = 0;
            src_NetworkNodeID = 0;
            ret = udsPullPacket(&bindctx, tmpbuf, tmpbuf_size, &actual_size, &src_NetworkNodeID);
            if(R_FAILED(ret))
            {
                printf("udsPullPacket() returned 0x%08x.\n", (unsigned int)ret);
                break;
            }

            if(actual_size)//If no data frame is available, udsPullPacket() will return actual_size=0.
            {
                printf("Received 0x%08x size=0x%08x from node 0x%x.\n", (unsigned int)tmpbuf[0], actual_size, (unsigned int)src_NetworkNodeID);
            }
        }

        if(udsWaitConnectionStatusEvent(false, false))
        {
            printf("Constatus event signaled.\n");
        }
    }

    free(tmpbuf);
    tmpbuf = NULL;

    terminateNetwork();
}

int main()
{
    gfxInitDefault();

    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    consoleSelect(&topScreen);

    printf("libctru UDS local-WLAN demo.\n");

    initUDS();
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