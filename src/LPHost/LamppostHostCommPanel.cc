/**
 * @filename: LP_CommPanel.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/16/2021
 */

#include "LPHost/LamppostHostCommPanel.hh"

#include <BATSSocket.h>

using namespace BATSProtocol;

void *LamppostHostSendThread(void *vargp) {
    auto args = (SendThreadArgs_t *) vargp;

    PRINTF_THREAD_STAMP("Sending thread has been launched, initializing BATSSocket and BATSParams.\n");
    BATSSocket socket;

    // Networking configurations
    int addr = parseNetAddrStr(args->hostProg->options.netaddr_str);
    int root_addr = parseNetAddrStr(args->hostProg->options.root_net_addr);
    int root_port = args->hostProg->options.root_port;

    // BATS Encoding Parameters
    int encMethod = BTP_BATS_ENC_SMALL;
    int batchSize = 16;
    auto param = new BATSEncParam(encMethod, batchSize, 8);
    int packetSize = BACKBONE_PACKET_SIZE;
    char *dataBuf = SMALLOC(char, packetSize);
    int dataLen = sizeof(LamppostBackbonePacket_t);

    if (packetSize < sizeof(LamppostBackbonePacket_t)) {
        PRINTF_THREAD_STAMP("Found Backbone Packet size is larger than the local data buffer size. Exit.\n");
        pthread_mutex_lock(&(args->hostProg->term_mutex));
        args->hostProg->term_flag = true;
        pthread_mutex_unlock(&(args->hostProg->term_mutex));
        return nullptr;
    }

    socket.init(addr);
    socket.bind(BACKBONE_SEND_PORT_DEFAULT);
    PRINTF_THREAD_STAMP("BATS Socket has been initialized.\n");

    while (!test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)) {
        // fill in databuf
        memset(dataBuf, 0, packetSize);
        LamppostBackbonePacket_t tmp_packet;
        tmp_packet.src_addr = addr;
        if (!args->hostProg->RoadBlockCoordinates.dequeue(tmp_packet.coord))
            break;
        memcpy(dataBuf, &tmp_packet, sizeof(LamppostBackbonePacket_t));

        // send databuf to root node
        if (socket.send(dataBuf, dataLen, root_addr, root_port, param, BP_PROTOCOL_BTP) == -1) {
            PRINTF_ERR_STAMP("Send packet error: %s\n", std::strerror(errno));
        } else {
            PRINTF_THREAD_STAMP("Sent 1 backbone packet to root node.\n");
        }

        usleep(BACKBONE_SEND_INTERVAL);
    }
    PRINTF_THREAD_STAMP("Catch termination flag, exit thread.\n");
    SFREE(dataBuf);
}


void *LamppostHostRecvThread(void *vargp) {
    auto args = (RecvThreadArgs_t *) vargp;

    if (args->hostProg->options.is_root_node) {
        // launch root program, receive RBCoordinates from slave nodes
        PRINTF_THREAD_STAMP("Root receiving thread is invoked\n");

        // Network configurations
        int root_addr = parseNetAddrStr(args->hostProg->options.root_net_addr);
        int root_port = args->hostProg->options.root_port;

        BATSSocket socket;
        socket.init(root_addr);
        socket.bind(root_port);
        PRINTF_THREAD_STAMP("BATS Socket has been initialized.\n");

        char *dataBuf = SMALLOC(char, BACKBONE_PACKET_SIZE);
        int dataLen;

        while (!test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)) {
            memset(dataBuf, 0, BACKBONE_PACKET_SIZE);
            dataLen = socket.recv(dataBuf, BACKBONE_PACKET_SIZE);
            LamppostBackbonePacket_t tmp_packet;
            memcpy(&tmp_packet, dataBuf, sizeof(LamppostBackbonePacket_t));
            PRINTF_THREAD_STAMP("Receive data from lamppost: %d\n", tmp_packet.src_addr);

            // Enqueue data into collected Roadblock Coordinates
            bool isNewRoadBlock = true;
            std::lock_guard<std::mutex> lock(args->hostProg->crb_mutex);
            for (int i = 0; i < args->hostProg->CollectedRBCoordinates.size(); i++) {
                auto distance = calculateDistance(tmp_packet.coord, args->hostProg->CollectedRBCoordinates[i]);
                if (distance >= RB_SEGMENT_THRESHOLD) {
                    isNewRoadBlock = false;
                    break;
                }
            }
            if (isNewRoadBlock) {
                PRINTF_THREAD_STAMP("Lamppost %d has detected a new road block at (%lf, %lf), "
                                    "appending into array: CollectedRBCoordinates.\n",
                                    tmp_packet.src_addr, tmp_packet.coord.latitude, tmp_packet.coord.longitude);
                args->hostProg->CollectedRBCoordinates.push_back(tmp_packet.coord);
            }
            args->hostProg->crb_c.notify_one();
        }
        SFREE(dataBuf);
    } else {
        // TODO: launch slave program, receive terminate flag from root node

    }
    PRINTF_THREAD_STAMP("Catch termination flag, exit thread\n");
}

/**
 * @function: LamppostHostCommHookSendThread
 * @description: This function initials a TCP socket and sends
 *              a list of detected road blocks to the hook node
 *              run on the RaspberryPI
 * @param vargp: virtual argument passed by pthread_create
 */
void *LamppostHostCommHookSendThread(void *vargp) {
    auto args = (HookSendThreadArgs_t *) vargp;
    auto hook_addr_str = args->hostProg->options.hook_ip_addr.c_str();
    auto hook_port = args->hostProg->options.hook_ip_port;

    // Create socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        PRINTF_ERR_STAMP("Create hook socket error. Should exit.\n");
        pthread_mutex_lock(&(args->hostProg->term_mutex));
        args->hostProg->term_flag = true;
        pthread_mutex_unlock(&(args->hostProg->term_mutex));
        return nullptr;
    }

    // Assign IP Address and Port
    struct sockaddr_in hook_addr_comp;
    hook_addr_comp.sin_family = AF_INET;
    hook_addr_comp.sin_port = htons(hook_port);
    if (inet_pton(AF_INET, hook_addr_str, &hook_addr_comp.sin_addr) <= 0) {
        PRINTF_ERR_STAMP("Invalid hook program address is provided. Should exit.\n");
        pthread_mutex_lock(&(args->hostProg->term_mutex));
        args->hostProg->term_flag = true;
        pthread_mutex_unlock(&(args->hostProg->term_mutex));
        return nullptr;
    }

    char *dataBuf = SMALLOC(char, HOOK_MAX_PACKET_SIZE);

    // Try to connect hook program
    while (connect(sockfd, (struct sockaddr *) &hook_addr_comp, sizeof(hook_addr_comp)) < 0) {
        PRINTF_THREAD_STAMP("Cannot connect to hook node, retry after %d seconds...\n", HOOK_CONN_RETRY_INTERVAL);
        sleep(1);
    }
    PRINTF_THREAD_STAMP("Connected to hook node, start sending coordinates and control flags.\n");

    while (!test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)) {
        memset(dataBuf, 0, HOOK_MAX_PACKET_SIZE);
        sleep(HOOK_PACKET_INTERVAL);
        if (args->hostProg->CollectedRBCoordinates.size() > HOOK_MAX_COORD_NUM) {
            PRINTF_ERR_STAMP("Error: Collected roadblocks exceeds the supported max num.\n");
            continue;
        }

        PRINTF_THREAD_STAMP("Communicating with hook node...\n");

        // Load content of collected RBCoordinates into send buffer
        HookPacket_t tmp_packet;
        memset(&tmp_packet, 0, sizeof(tmp_packet));
        tmp_packet.ctrl_zigbee_pan = args->hostProg->options.ctrl_zigbee_pan;
        tmp_packet.ctrl_zigbee_addr = args->hostProg->options.ctrl_zigbee_addr;
        tmp_packet.root_zigbee_pan = args->hostProg->options.root_zigbee_pan;
        tmp_packet.root_zigbee_addr = args->hostProg->options.root_zigbee_addr;
        tmp_packet.flag = (args->hostProg->term_flag);

        std::lock_guard<std::mutex> sendlock(args->hostProg->crb_mutex);
        tmp_packet.coords_num = args->hostProg->CollectedRBCoordinates.size();
        for (int i = 0; i < tmp_packet.coords_num; i++) {
            tmp_packet.coords[i] = args->hostProg->CollectedRBCoordinates[i];
        }
        args->hostProg->crb_c.notify_one();
        memcpy(dataBuf, &tmp_packet, sizeof(HookPacket_t));
        send(sockfd, dataBuf, HOOK_MAX_PACKET_SIZE, 0);
        PRINTF_THREAD_STAMP("Sent 1 packet to hook node.\n");
    }
    SFREE(dataBuf);
    PRINTF_THREAD_STAMP("Catch termination flag, exit thread.\n");
}