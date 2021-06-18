/**
 * @filename: LP_RBDetection.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#include "LamppostHostRBDetection.hh"

void* RBDetectionThread(void *vargp) {
    auto args = (RBDetectionThreadArgs_t *) vargp;
}

void* RBDetectionMockThread(void* vargp) {
    auto args = (RBDetectionThreadArgs_t*) vargp;
    while (true) {
        PRINTF_THREAD_STAMP("Mock add RBCoordinate into queue\n");
        sleep(1);
    }
}