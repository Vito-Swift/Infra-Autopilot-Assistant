/**
 * @filename: LP_CommPanel.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 4/16/2021
 */

#include "LamppostHostCommPanel.hh"
#include <BATSSocket.h>

using namespace BATSProtocol;

// TODO: functions in this file aim to provide following supports
//  1. send retrieved GPS coordinate of road blocks to root node
//  2. send all GPS coordinates to hook node (for root node only)

void LamppostHostCommCoordSender(LamppostHostProg* prog) {
    BATSSocket socket;
    socket.init();
}

void LamppostHostCommHookPan(LamppostHostProg* prog) {

}