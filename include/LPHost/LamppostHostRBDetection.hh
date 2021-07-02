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

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

// include from stdc++
#include <iostream>
#include <memory>
#include <numeric>
#include <atomic>

// include from opencv lib
#include <opencv2/highgui/highgui.hpp>

// include from aruco lib
#include <aruco/cameraparameters.h>
#include <aruco/dictionary.h>
#include <aruco/markerlabeler.h>

// include from marker mapper lib
#include "marker_mapper/markermapper.h"

typedef struct RBDetectionThreadArgs {
    LamppostHostProg *hostProg;
    std::string cam_addr;
    int ref_marker_id;
} RBDetectionThreadArgs_t;

void *RBDetectionThread(void *vargp);

void *RBDetectionMockThread(void *vargp);

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTRBDETECTION_HH
