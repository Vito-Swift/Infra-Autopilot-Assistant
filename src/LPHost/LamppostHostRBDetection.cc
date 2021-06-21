/**
 * @filename: LP_RBDetection.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#include "LamppostHostRBDetection.hh"

void *RBDetectionThread(void *vargp) {
    auto args = (RBDetectionThreadArgs_t *) vargp;
}

void *RBDetectionMockThread(void *vargp) {
    auto args = (RBDetectionThreadArgs_t *) vargp;
    while (!(*args->terminate_flag)) {
        PRINTF_THREAD_STAMP("Mock add RBCoordinate into queue\n");
        args->hostProg->RoadBlockCoordinates.enqueue(RBCoordinate(0, 0));
        sleep(1);
    }
    PRINTF_THREAD_STAMP("Catch termination flag, exit thread\n");
}
