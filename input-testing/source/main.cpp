#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include <3ds.h>

PrintConsole topScreen, bottomScreen;

Result ret = 0;
u32 con_type=  0;

u8 data_channel = 1;
udsNetworkStruct networkstruct;
udsBindContext bindctx;
udsNetworkScanInfo *networks = NULL;
udsNetworkScanInfo *network = NULL;

size_t total_networks = 0;

u32 recv_buffer_size = UDS_DEFAULT_RECVBUFSIZE;
u32 wlancommID = 0x48424200;
char *passphrase = "You're a squid now! You're a kid now!";

udsConnectionType conntype = UDSCONTYPE_Client;

u32 transfer_data, prev_transfer_data = 0;
size_t actual_size;
u16 src_NetworkNodeID;
u32 tmp = 0;

udsNodeInfo tmpnode;

u8 appdata[0x14] = {0x69, 0x8a, 0x05, 0x5c};
u8 out_appdata[0x14];

char tmpstr[256];

void print_constatus()
{
    Result ret=0;
    udsConnectionStatus constatus;

    //By checking the output of udsGetConnectionStatus you can check for nodes (including the current one) which just (dis)connected, etc.
    ret = udsGetConnectionStatus(&constatus);
    if(R_FAILED(ret))
    {
        printf("udsGetConnectionStatus() returned 0x%08x.\n", (unsigned int)ret);
    }
    else
    {
        printf("constatus:\nstatus=0x%x\n", (unsigned int)constatus.status);
        printf("1=0x%x\n", (unsigned int)constatus.unk_x4);
        printf("cur_NetworkNodeID=0x%x\n", (unsigned int)constatus.cur_NetworkNodeID);
        printf("unk_xa=0x%x\n", (unsigned int)constatus.unk_xa);
        for(u32 i = 0; i<(0x20>>2); i++)printf("%u=0x%x ", (unsigned int)i+3, (unsigned int)constatus.unk_xc[i]);
        printf("\ntotal_nodes=0x%x\n", (unsigned int)constatus.total_nodes);
        printf("max_nodes=0x%x\n", (unsigned int)constatus.max_nodes);
        printf("node_bitmask=0x%x\n", (unsigned int)constatus.total_nodes);
    }
}

size_t searchForNetworks(u32 iterations) {
    u32 *tmpbuf;
    size_t tmpbuf_size;

    size_t network_count = 0;

    strncpy((char*)&appdata[4], "Test appdata.", sizeof(appdata)-4);

    printf("Successfully initialized.\n");

    tmpbuf_size = 0x4000;
    tmpbuf = (u32*)malloc(tmpbuf_size);
    if(tmpbuf==NULL)
    {
        printf("Failed to allocate tmpbuf for beacon data.\n");
        return network_count;
    }

    //With normal client-side handling you'd keep running network-scanning until the user chooses to stops scanning or selects a network to connect to. This example just scans a maximum of 10 times until at least one network is found.
    for(u32 i = 0; i < iterations; i++)
    {
        memset(tmpbuf, 0, sizeof(tmpbuf_size));
        udsScanBeacons(tmpbuf, tmpbuf_size, &networks, &network_count, wlancommID, 0, NULL, false);
        printf("total_networks=%u.\n", (unsigned int)network_count);
        //if(total_networks)break;
    }

    free(tmpbuf);
    tmpbuf = NULL;

    return network_count;
}

void getNetworkAppData(int index) {
    char tmpstr[256];
    udsNetworkScanInfo *tmpNetwork = &networks[index];
    actual_size = 0;

    ret = udsGetNetworkStructApplicationData(&tmpNetwork->network, out_appdata, sizeof(out_appdata), &actual_size);

    if(R_FAILED(ret) || actual_size!=sizeof(out_appdata))
    {
        printf("udsGetNetworkStructApplicationData() returned 0x%08x. actual_size = 0x%x.\n", (unsigned int)ret, actual_size);
        free(networks);
        return;
    }

    memset(tmpstr, 0, sizeof(tmpstr));
    if(memcmp(out_appdata, appdata, 4)!=0)
    {
        printf("The first 4-bytes of appdata is invalid.\n");
        free(networks);
        return;
    }

    strncpy(tmpstr, (char*)&out_appdata[4], sizeof(out_appdata)-5);
    tmpstr[sizeof(out_appdata)-6]='\0';

    printf("App data: %s", (char*)&out_appdata[4]);
}

void connectToNetwork(int index, bool spectator) {
    //At this point you'd let the user select which network to connect to and optionally display the first node's username(the host),
    // along with the parsed appdata if you want. For this example this just uses the first detected network and then displays the username of each node.
    //If appdata isn't enough, you can do what DLP does loading the icon data etc: connect to the network as a spectator temporarily for receiving broadcasted data frames.

    if(!total_networks)
        return;
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
            return;
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
        ret = udsConnectNetwork(&network->network, passphrase, strlen(passphrase)+1, &bindctx, UDS_BROADCAST_NETWORKNODEID, conntype, data_channel, recv_buffer_size);
        if(R_FAILED(ret))
        {
            printf("udsConnectNetwork() returned 0x%08x.\n", (unsigned int)ret);
        }
        else
        {
            break;
        }
    }

    free(networks);

    printf("Connected.\n");
}

void createNetwork() {
    udsGenerateDefaultNetworkStruct(&networkstruct, wlancommID, 0, UDS_MAXNODES);

    printf("Creating the network...\n");
    ret = udsCreateNetwork(&networkstruct, passphrase, strlen(passphrase)+1, &bindctx, data_channel, recv_buffer_size);
    if(R_FAILED(ret))
    {
        printf("udsCreateNetwork() returned 0x%08x.\n", (unsigned int)ret);
        return;
    }

    ret = udsSetApplicationData(appdata, sizeof(appdata));//If you want to use appdata, you can set the appdata whenever you want after creating the network. If you need more space for appdata, you can set different chunks of appdata over time.
    if(R_FAILED(ret))
    {
        printf("udsSetApplicationData() returned 0x%08x.\n", (unsigned int)ret);
        udsDestroyNetwork();
        udsUnbind(&bindctx);
        return;
    }

    tmp = 0;
    ret = udsGetChannel((u8*)&tmp);//Normally you don't need to use this.
    printf("udsGetChannel() returned 0x%08x. channel = %u.\n", (unsigned int)ret, (unsigned int)tmp);
    if(R_FAILED(ret))
    {
        udsDestroyNetwork();
        udsUnbind(&bindctx);
        return;
    }

    con_type = 0;
}

void printNetworksMenu(int index) {
    consoleSelect(&bottomScreen);
    printf("%x Available Networks:\n", total_networks);
    for(size_t i = 0; i < total_networks; i++) {
        printf("========================================\n");
        if(i == index) printf(">");
        else printf("|");
        getNetworkAppData(i);
        printf("|\n\n");
    }
    printf("========================================\n");
    consoleSelect(&topScreen);
}

void uds_test()
{
    u32 *tmpbuf;
    size_t tmpbuf_size;

    printf("Start network: A\nStart Search: B\n");
    while(1) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if(kDown & KEY_A) {
            createNetwork();
            break;
        }
        if(kDown & KEY_B) {
            total_networks = searchForNetworks(5);
            printNetworksMenu(0);
            break;
        }
        if(kDown & KEY_Y) {
            iprintf("\x1b[2J");
        }
    }

    if(total_networks)
    {
        //You can load appdata from the scanned beacon data here if you want.

        connectToNetwork(0, false);

        tmp = 0;
        ret = udsGetChannel((u8*)&tmp);//Normally you don't need to use this.
        printf("udsGetChannel() returned 0x%08x. channel = %u.\n", (unsigned int)ret, (unsigned int)tmp);
        if(R_FAILED(ret))
        {
            return;
        }

        //You can load the appdata with this once connected to the network, if you want.
        memset(out_appdata, 0, sizeof(out_appdata));
        actual_size = 0;
        ret = udsGetApplicationData(out_appdata, sizeof(out_appdata), &actual_size);
        if(R_FAILED(ret) || actual_size!=sizeof(out_appdata))
        {
            printf("udsGetApplicationData() returned 0x%08x. actual_size = 0x%x.\n", (unsigned int)ret, actual_size);
            udsDisconnectNetwork();
            udsUnbind(&bindctx);
            return;
        }

        memset(tmpstr, 0, sizeof(tmpstr));
        if(memcmp(out_appdata, appdata, 4)!=0)
        {
            printf("The first 4-bytes of appdata is invalid.\n");
            udsDisconnectNetwork();
            udsUnbind(&bindctx);
            return;
        }

        strncpy(tmpstr, (char*)&out_appdata[4], sizeof(out_appdata)-5);
        tmpstr[sizeof(out_appdata)-6]='\0';

        printf("String from appdata: %s\n", (char*)&out_appdata[4]);

        con_type = 1;
    }

    if(udsWaitConnectionStatusEvent(false, false))
    {
        printf("Constatus event signaled.\n");
        print_constatus();
    }

    printf("Press A to stop data transfer.\n");

    tmpbuf_size = UDS_DATAFRAME_MAXSIZE;
    tmpbuf = (u32*)malloc(tmpbuf_size);
    if(tmpbuf==NULL)
    {
        printf("Failed to allocate tmpbuf for receiving data.\n");

        if(con_type)
        {
            udsDestroyNetwork();
        }
        else
        {
            udsDisconnectNetwork();
        }
        udsUnbind(&bindctx);

        return;
    }

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

        if(kDown & KEY_Y)
        {
            ret = udsGetNodeInformation(0x2, &tmpnode);//This can be used to get the NodeInfo for a node which just connected, for example.
            if(R_FAILED(ret))
            {
                printf("udsGetNodeInformation() returned 0x%08x.\n", (unsigned int)ret);
            }
            else
            {
                memset(tmpstr, 0, sizeof(tmpstr));

                ret = udsGetNodeInfoUsername(&tmpnode, tmpstr);
                if(R_FAILED(ret))
                {
                    printf("udsGetNodeInfoUsername() returned 0x%08x for udsGetNodeInfoUsername.\n", (unsigned int)ret);
                }
                else
                {
                    printf("node username: %s\n", tmpstr);
                    printf("node unk_x1c=0x%x\n", (unsigned int)tmpnode.unk_x1c);
                    printf("node flag=0x%x\n", (unsigned int)tmpnode.flag);
                    printf("node pad_x1f=0x%x\n", (unsigned int)tmpnode.pad_x1f);
                    printf("node NetworkNodeID=0x%x\n", (unsigned int)tmpnode.NetworkNodeID);
                    printf("node word_x24=0x%x\n", (unsigned int)tmpnode.word_x24);
                }
            }
        }

        if(kDown & KEY_X)//Block new regular clients from connecting.
        {
            ret = udsSetNewConnectionsBlocked(true, true, false);
            printf("udsSetNewConnectionsBlocked() for enabling blocking returned 0x%08x.\n", (unsigned int)ret);
        }

        if(kDown & KEY_B)//Unblock new regular clients from connecting.
        {
            ret = udsSetNewConnectionsBlocked(false, true, false);
            printf("udsSetNewConnectionsBlocked() for disabling blocking returned 0x%08x.\n", (unsigned int)ret);
        }

        if(kDown & KEY_R)
        {
            ret = udsEjectSpectator();
            printf("udsEjectSpectator() returned 0x%08x.\n", (unsigned int)ret);
        }

        if(kDown & KEY_L)
        {
            ret = udsAllowSpectators();
            printf("udsAllowSpectators() returned 0x%08x.\n", (unsigned int)ret);
        }

        if(udsWaitConnectionStatusEvent(false, false))
        {
            printf("Constatus event signaled.\n");
            print_constatus();
        }
    }

    free(tmpbuf);
    tmpbuf = NULL;

    if(con_type)
    {
        udsDestroyNetwork();
    }
    else
    {
        udsDisconnectNetwork();
    }
    udsUnbind(&bindctx);
}

int main()
{
    Result ret=0;

    gfxInitDefault();

    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    consoleSelect(&topScreen);

    printf("libctru UDS local-WLAN demo.\n");

    ret = udsInit(0x3000, NULL);//The sharedmem size only needs to be slightly larger than the total recv_buffer_size for all binds, with page-alignment.
    if(R_FAILED(ret))
    {
        printf("udsInit failed: 0x%08x.\n", (unsigned int)ret);
    }
    else
    {
        uds_test();
        udsExit();
    }

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