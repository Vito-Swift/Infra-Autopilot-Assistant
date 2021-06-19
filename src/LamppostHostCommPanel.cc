/**
 * @filename: LP_CommPanel.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/16/2021
 */

#include "LamppostHostCommPanel.hh"
#include <BATSSocket.h>

#define BACKBONE_PACKET_SIZE 3000UL
#define BACKBONE_SEND_PORT_DEFAULT 201
#define BACKBONE_RECV_PORT_DEFAULT 200
#define BACKBONE_SEND_INTERVAL 100UL

using namespace BATSProtocol;

typedef struct LamppostBackbonePacket {
    RBCoordinate coord;
    uint32_t src_addr;
    bool _terminate;
} LamppostBackbonePacket_t;

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
        *(args->terminate_flag) = true;
        return nullptr;
    }

    socket.init(addr);
    socket.bind(BACKBONE_SEND_PORT_DEFAULT);
    PRINTF_THREAD_STAMP("BATS Socket has been initialized.\n");

    while (!*(args->terminate_flag)) {
        // fill in databuf
        LamppostBackbonePacket_t tmp_packet;
        tmp_packet.src_addr = addr;
        tmp_packet.coord = args->hostProg->RoadBlockCoordinates.dequeue();
        memcpy(dataBuf, &tmp_packet, sizeof(LamppostBackbonePacket_t));

        // send databuf to root node
        if (socket.send(dataBuf, dataLen, root_addr, root_port, param, BP_PROTOCOL_BTP) == -1) {
            PRINTF_ERR_STAMP("Send packet error: %s\n", std::strerror(errno));
        } else {
            PRINTF_THREAD_STAMP("Sent backbone packet to root node.\n");
        }

        usleep(BACKBONE_SEND_INTERVAL);
    }
    PRINTF_THREAD_STAMP("Catch termination flag, exit thread\n");
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

        while (!*(args->terminate_flag)) {
            dataLen = socket.recv(dataBuf, BACKBONE_PACKET_SIZE);
            LamppostBackbonePacket_t tmp_packet;
            memcpy(&tmp_packet, dataBuf, sizeof(LamppostBackbonePacket_t));
            PRINTF_THREAD_STAMP("Receive data from lamppost: %d\n", tmp_packet.src_addr);
        }
    } else {
        // launch slave program, receive terminate flag from root node

    }
    PRINTF_THREAD_STAMP("Catch termination flag, exit thread\n");
}

void LamppostHostCommHookPan(LamppostHostProg *prog) {

}