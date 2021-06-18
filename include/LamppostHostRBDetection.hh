/**
 * @filename: LP_RBDetection.hh
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTRBDETECTION_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTRBDETECTION_HH

#include "LamppostHostUtils.hh"

typedef struct RBDetectionThreadArgs {
    LamppostHostProg* hostProg;
} RBDetectionThreadArgs_t;

void* RBDetectionThread(void* vargp);
void* RBDetectionMockThread(void* vargp);

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTRBDETECTION_HH
