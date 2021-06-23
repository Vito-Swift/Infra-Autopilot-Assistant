//
// Created by Vito Wu on 2021/6/16.
//

#include "LPHook/LamppostHookCommPanel.hh"

void *HookRecvThread(void *vargp) {
    auto args = (HookRecvArgs_t *) vargp;
    int sockfd, connfd, len;
    int opt = 1;
    struct sockaddr_in hook_addr;

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        PRINTF_ERR_STAMP("Receive TCP socket creation failed...\n");
        *args->termination_flag = true;
        exit(0);
    } else {
        PRINTF_THREAD_STAMP("Socket successfully created...\n");
    }
    bzero(&hook_addr, sizeof(hook_addr));

    // forcefully set listen port to args->port
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        PRINTF_ERR_STAMP("Fail to set socket options\n");
        *args->termination_flag = true;
        exit(0);
    }

    // assign IP and listening port
    hook_addr.sin_family = AF_INET;
    hook_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    hook_addr.sin_port = htons(args->port);

    // bind port
    while ((bind(sockfd, (struct sockaddr *) &hook_addr, sizeof(hook_addr))) != 0) {
        PRINTF_THREAD_STAMP("socket bind failed, retry after 1 sec...\n");
        sleep(1);
    }

    // listen port for incoming connection request
    if ((listen(sockfd, 100)) != 0) {
        PRINTF_ERR_STAMP("Receive TCP socket listen failed...\n");
        *args->termination_flag = true;
        exit(0);
    } else {
        PRINTF_THREAD_STAMP("Receive socket listening...\n");
    }

    // accept connection request from host program
    len = sizeof(hook_addr);
    if ((connfd = accept(sockfd, (struct sockaddr *) &hook_addr, (socklen_t *) &len)) != 0) {
        PRINTF_ERR_STAMP("Fail to accept TCP connection from host program.\n");
        *args->termination_flag = true;
        exit(0);
    }

    PRINTF_THREAD_STAMP("TCP connection between hook and host has been established!\n");

    char *dataBuf = SMALLOC(char, HOOK_MAX_PACKET_SIZE);
    while (!*args->termination_flag) {
        bzero(dataBuf, HOOK_MAX_PACKET_SIZE);
        int valread = read(connfd, dataBuf, HOOK_MAX_PACKET_SIZE);
        auto tmp_packet = (HookPacket_t *) dataBuf;

        // Receive 1 packet from host program
        PRINTF_THREAD_STAMP("Received 1 hook packet from host...\n");
        args->PacketQueue->enqueue(*tmp_packet);
        usleep(10);
    }
    SFREE(dataBuf);
}

void *HookSendThread(void *vargp) {

}
