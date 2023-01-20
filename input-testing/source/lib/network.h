/*
 * Wrapper library for communication via UDS
 * on the 3DS
 *
 * Written By: Jemma Poffinbarger
 * Last Updated: January 19th, 2023
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <3ds.h>

namespace Network {
    class UDS {
    private:
        const u32 receive_buffer_size = UDS_DEFAULT_RECVBUFSIZE;
        const u32 wlancommID = 0x48424200;
        const u8 data_channel = 1;
        const char AppDataString[0x11] = "Splatoon 3DS";
        const char *passphrase = "You're a squid now! You're a kid now!";
        _Static_assert(18 > sizeof(AppDataString), "AppDataString too long! Must be less than 16 chars.");
        u8 appdata[0x14] = {0x69, 0x8a, 0x05, 0x5c};
        u32 con_type =  0; // 0 host 1 client
        udsBindContext bindctx;
        udsNetworkScanInfo *networks = NULL;
        udsNetworkScanInfo *network = NULL;
        udsConnectionType conntype = UDSCONTYPE_Client;
        size_t total_networks = 0, total_nodes = 0;

    public:
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
         * Creates a new network
         */
        Result createNetwork() {
            Result ret = 0;
            udsNetworkStruct networkstruct;
            udsGenerateDefaultNetworkStruct(&networkstruct, wlancommID, 0, UDS_MAXNODES);
            #pragma GCC diagnostic ignored "-Wstringop-truncation"
            strncpy((char*)&appdata[4], AppDataString, sizeof(appdata)-4);

            printf("Creating the network...\n");
            ret = udsCreateNetwork(&networkstruct, passphrase, strlen(passphrase)+1, &bindctx, data_channel, receive_buffer_size);
            if(R_FAILED(ret))
            {
                printf("udsCreateNetwork() failed: 0x%08x.\n", (unsigned int)ret);
                return ret;
            }

            ret = udsSetApplicationData(appdata, sizeof(appdata));
            if(R_FAILED(ret))
            {
                printf("udsSetApplicationData() failed: 0x%08x.\n", (unsigned int)ret);
                udsDestroyNetwork();
                udsUnbind(&bindctx);
                return ret;
            }
            con_type = 0;
            return ret;
        }

        /**
         * Connects to a network by it's index either as a client
         * or a spectator.
         * @param index
         * @param spectator
         */
        Result connectToNetwork(int index, bool spectator) {
            Result ret = 0;
            if(!total_networks)
                return ret;
            network = &networks[index];

            if(spectator)
            {
                conntype = UDSCONTYPE_Spectator;
                printf("[Info: connectToNetwork] Connecting to the network as a spectator...\n");
            }
            else
                printf("[Info: connectToNetwork] Connecting to the network as a client...\n");
            for(u32 i = 0; i < 10; i++)
            {
                ret = udsConnectNetwork(&network->network, passphrase, strlen(passphrase)+1, &bindctx, UDS_BROADCAST_NETWORKNODEID, conntype, data_channel, receive_buffer_size);
                if(R_FAILED(ret))
                {
                    printf("[Error: connectToNetwork] udsConnectNetwork() returned %u.\n", (unsigned int)ret);
                    printf("[Error: connectToNetwork] Unable to connect to selected network.\n");
                    return ret;
                }
                else break;
            }

            free(networks);
            con_type = 1;
            printf("[Info: connectToNetwork] Connected.\n");
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
            if(tmpbuf == NULL)
            {
                printf("[Error: searchForNetworks] Failed to allocate tmpbuf for beacon data.\n");
                return -1;
            }
            for(int i = 0; i < iterations; i++)
            {
                memset(tmpbuf, 0, sizeof(tmpbuf_size));
                udsScanBeacons(tmpbuf, tmpbuf_size, &networks, &network_count, wlancommID, 0, NULL, false);
            }
            free(tmpbuf);
            tmpbuf = NULL;
            total_networks = network_count;
            return network_count;
        }

        /**
         * Returns number of available networks
         * @return total_networks
         */
        size_t getNumberOfNetworks() { return total_networks; }

        /**
         * Gets the appdata for the specified network
         * @param index index of network to parse app data from
         * @return appdata
         */
        std::vector<char> getNetworkAppData(int index) {
            Result ret = 0;
            u8 out_appdata[0x14];
            udsNetworkScanInfo *tmpNetwork = &networks[index];
            size_t actual_size = 0;

            ret = udsGetNetworkStructApplicationData(&tmpNetwork->network, out_appdata, sizeof(out_appdata), &actual_size);

            if(R_FAILED(ret) || actual_size!=sizeof(out_appdata))
            {
                printf("[Error: getNetworkAppData] udsGetNetworkStructApplicationData() returned 0x%08x.\n", (unsigned int)ret);
                free(networks);
            }
            if(memcmp(out_appdata, appdata, 4)!=0)
            {
                printf("[Error: getNetworkAppData] The first 4-bytes of appdata is invalid.\n");
                free(networks);
            }
            std::vector<char> output(0x14);
            strcpy(output.data(), (char*)&out_appdata[4]);
            return output;
        }

        /**
         * Get a specified network's host username
         * @param index
         * @return username
         */
        std::vector<char> getNetworkOwnerUsername(u16 index) {
            Result ret = 0;
            std::vector<char> username(0x0b);
            if(!udsCheckNodeInfoInitialized(&networks[index].nodes[0]))
                return username;

            ret = udsGetNodeInfoUsername(&networks[index].nodes[0], username.data());
            if(R_FAILED(ret))
            {
                printf("[Error: getNetworkOwnerUsername] udsGetNodeInfoUsername() returned 0x%08x.\n", (unsigned int)ret);
                free(networks);
            }

            return username;
        }

        /**
         * Get a specified client's username
         * @param index
         * @return username
         */
        std::vector<char> getNodeUsername(u16 index) {
            Result ret = 0;
            std::vector<char> username(0x0b);
            udsNodeInfo output;
            ret = udsGetNodeInformation(index, &output);
            if(R_FAILED(ret))
                printf("[Error: getNodeUsername] udsGetNodeInformation() returned 0x%08x.\n", (unsigned int)ret);
            ret = udsGetNodeInfoUsername(&output, username.data());
            if(R_FAILED(ret))
                printf("[Error: getNodeUsername] udsGetNodeInfoUsername() returned 0x%08x.\n", (unsigned int)ret);
            return username;
        }

        /**
         * Sends packet to either all or one client
         * @param buffer
         * @param node
         * @return Result
         */
        Result sendPacket(void * buffer, size_t buffLen, u16 node) {
            Result ret = 0;
            if(conntype == UDSCONTYPE_Spectator)
                return -3;
            if(!node)
                node = UDS_BROADCAST_NETWORKNODEID;
            ret = udsSendTo(node, data_channel, UDS_SENDFLAG_Default, buffer, buffLen);
            return ret;
        }

        /**
         * Pulls the latest packet from the buffer
         * @param src_NetworkNodeID
         * @return Packet buffer
         */
        std::vector<uint8_t> pullPacket(u16 &src_NetworkNodeID) {
            Result ret = 0;
            src_NetworkNodeID = 0;
            size_t actual_size = 0;
            std::vector<uint8_t> uds_buffer(UDS_DATAFRAME_MAXSIZE);

            ret = udsPullPacket(&bindctx, uds_buffer.data(), uds_buffer.size(), &actual_size, &src_NetworkNodeID);
            if(R_FAILED(ret))
                printf("udsPullPacket() returned 0x%08x.\n", (unsigned int)ret);
            if(actual_size)
                uds_buffer.resize(actual_size);

            return uds_buffer;
        }

        /**
         * Waits for the bind event to occur, or checks if the event was signaled.
         * @param nextEvent
         * @param wait
         * @return packet availability
         */
        bool packetAvailable(bool nextEvent, bool wait) { return udsWaitDataAvailable(&bindctx, nextEvent, wait); }

        /**
         * Waits for the ConnectionStatus event to occur, or checks if the event was signaled.
         * @param nextEvent
         * @param wait
         * @return
         */
        bool connectionStatusAvailable(bool nextEvent, bool wait) { return udsWaitConnectionStatusEvent(nextEvent, wait); }

        /**
         * Get udsConnectionStatus struct
         * @return udsConnectionStatus
         */
        udsConnectionStatus getConnStatus() {
            Result ret = 0;
            udsConnectionStatus constatus;
            ret = udsGetConnectionStatus(&constatus);
            if(R_FAILED(ret))
                printf("udsGetConnectionStatus() returned 0x%08x.\n", (unsigned int)ret);
            return constatus;
        }

        /**
         * Determines what the status event was
         * @param constatus
         * @return status
         */
        int parseStatus(udsConnectionStatus constatus) {
            /*
             * -1: Unknown state
             *  1: Network created
             *  2: Join network
             *  3: Client connected to host
             *  4: Client disconnected from host
             *  5: Host network closed
             */
            switch (constatus.status) {
                case 3:
                    return 5;
                case 6:
                    if(total_nodes == 0) {
                        total_nodes = constatus.total_nodes;
                        return 1;
                    }
                    else if(constatus.total_nodes > total_nodes) {
                        total_nodes = constatus.total_nodes;
                        return 3;
                    }
                    else if(constatus.total_nodes < total_nodes) {
                        total_nodes = constatus.total_nodes;
                        return 4;
                    }
                    else
                        return -1;
                case 9:
                    return 2;
                default:
                    return -1;
            }
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
         * @return Result
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
    };
}