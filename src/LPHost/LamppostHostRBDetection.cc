/**
 * @filename: LP_RBDetection.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#include "LPHost/LamppostHostRBDetection.hh"

typedef struct {
    float long_diff_in_xm;
    float long_diff_in_ym;
    float lat_diff_in_xm;
    float lat_diff_in_ym;
} geometrics_t;

cv::Point3f mean3f(const std::vector <cv::Point3f> &points) {
    cv::Point3f sum = std::accumulate(points.begin(), points.end(),
                                      cv::Point3f(0.0f, 0.0f, 0.0f),
                                      std::plus<cv::Point3f>());
    cv::Point3f mean(sum.x / points.size(), sum.y / points.size(), sum.z / points.size());
    return mean;
}

geometrics_t getMetricsFromXML(const std::string &ref_file) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_xml(ref_file, pt);
    geometrics_t ret;

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("RefPoints")) {
                    if (v.first == "Metric") {
                        ret.lat_diff_in_xm = v.second.get<float>("lat_diff_in_xm", 1.0);
                        ret.lat_diff_in_ym = v.second.get<float>("lat_diff_in_ym", 1.0);
                        ret.long_diff_in_xm = v.second.get<float>("long_diff_in_xm", 1.0);
                        ret.long_diff_in_ym = v.second.get<float>("long_diff_in_ym", 1.0);
                    }
                }
    return ret;
}

RBCoordinate getRefCoordinateFromXML(int ref_id, const std::string &ref_file) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_xml(ref_file, pt);
    auto key = std::to_string(ref_id);
    bool found = false;
    RBCoordinate ret;

    // iterate over GPS reference file to find the entry with aruco id equals to ref id
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("RefPoints")) {
                    if (v.first == "RefPoint" && v.second.get<int>("aruco_id", -1) == ref_id) {
                        found = true;
                        ret.latitude = v.second.get<float>("latitude", RB_DEFAULT_REF_LATI);
                        ret.longitude = v.second.get<float>("longitude", RB_DEFAULT_REF_LONG);
                    }
                }

    if (!found) {
        PRINTF_THREAD_STAMP("Cannot get GPS information of reference marker from the given xml file.\n");
        PRINTF_THREAD_STAMP("Use default longitude and latitude value instead.\n");
        ret.longitude = RB_DEFAULT_REF_LONG;
        ret.longitude = RB_DEFAULT_REF_LATI;
    }

    return ret;
}

// estimate the gps coordinate of detected road block with respect to the reference aruco marker
RBCoordinate estimateRBCoordinate(const RBCoordinate &refCoordinate,
                                  const geometrics_t geometrics,
                                  const float delta_x,
                                  const float delta_y,
                                  const float delta_z) {
    return {
            refCoordinate.latitude + delta_x * geometrics.lat_diff_in_xm + delta_z * geometrics.lat_diff_in_ym,
            refCoordinate.longitude + delta_x * geometrics.long_diff_in_xm + delta_z * geometrics.long_diff_in_ym};
}

void *RBDetectionThread(void *vargp) {
    auto args = (RBDetectionThreadArgs_t *) vargp;
    std::string camera_param_file = args->hostProg->options.calibration_file;
    float marker_size = args->hostProg->options.marker_size;
    int ref_marker_id = args->ref_marker_id;
    int frame_increment = args->hostProg->options.frame_increment;

    PRINTF_THREAD_STAMP("%s \t Get Reference ArUco maker id: %d\n", args->cam_addr.c_str(), ref_marker_id);
    RBCoordinate ref_gps = getRefCoordinateFromXML(ref_marker_id, args->hostProg->options.ref_gps_file);
    PRINTF_THREAD_STAMP("%s \t GPS Coordinate of reference marker: latitude %lf \t longitude %f\n",
                        args->cam_addr.c_str(), ref_gps.latitude, ref_gps.longitude);
    geometrics_t metrics = getMetricsFromXML(args->hostProg->options.ref_gps_file);

    PRINTF_THREAD_STAMP("%s \t Opening video stream......\n", args->cam_addr.c_str());
    cv::VideoCapture cap;
    cap.open(args->cam_addr);
    cv::Mat frame;

    PRINTF_THREAD_STAMP("%s \t Configure marker mapper......\n", args->cam_addr.c_str());
    aruco::CameraParameters camera_params;
    camera_params.readFromXMLFile(camera_param_file);

    PRINTF_THREAD_STAMP("%s \t Start to map ArUco markers in the environment\n", args->cam_addr.c_str());
    float total_batch_num = 0;
    float fail_batch_num = 0;
    do {
        cap >> frame;
        if (frame.empty()) {
            PRINTF_THREAD_STAMP("%s \t video stream interrupted, waiting...\n", args->cam_addr.c_str());
            sleep(3);
            continue;
        }

        if (test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)) {
            PRINTF_THREAD_STAMP("Catch termination flag!\n");
            pthread_exit(nullptr);
        }

        PRINTF_THREAD_STAMP("%s \t Preparing data structures\n", args->cam_addr.c_str());
        std::shared_ptr<aruco_mm::MarkerMapper> arucoMarkerMapper;
        arucoMarkerMapper = aruco_mm::MarkerMapper::create();
        arucoMarkerMapper->setParams(camera_params, marker_size, ref_marker_id);
        arucoMarkerMapper->getMarkerDetector().setDictionary("ARUCO");

        total_batch_num += 1.0;
        PRINTF_THREAD_STAMP("%s \t Start processing on the subsequent batch of frames...\n", args->cam_addr.c_str());
        for (int i = 0;
             i < frame_increment && !(test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)); i++) {
            cap >> frame;
            if (frame.empty())
                break;
            arucoMarkerMapper->process(frame, i, true);
        }

        if (test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)) {
            PRINTF_THREAD_STAMP("Catch termination flag!\n");
            pthread_exit(nullptr);
        }

        PRINTF_THREAD_STAMP("%s \t Main processing has done\n", args->cam_addr.c_str());
        PRINTF_THREAD_STAMP("%s \t Optimizing mapping result...\n", args->cam_addr.c_str());
        try {
            arucoMarkerMapper->optimize();
            std::vector<int> detectedArucoMarkerIDList;
            arucoMarkerMapper->getMarkerMap().getIdList(detectedArucoMarkerIDList);

            uint detected_marker_num = detectedArucoMarkerIDList.size();
            PRINTF_THREAD_STAMP("%s \t Detected %d markers in the video\n", args->cam_addr.c_str(),
                                detected_marker_num);
            for (int i : detectedArucoMarkerIDList) {
                cv::Point3f mean =
                        mean3f(arucoMarkerMapper->getMarkerMap().getMarker3DInfo(i).points);
                RBCoordinate coordinate = estimateRBCoordinate(ref_gps, metrics, mean.x, mean.y, mean.z);
                PRINTF_THREAD_STAMP("%s \t Aruco index: %d \t At: (%lf, %lf, %lf)\t estimate gps: (%lf, %lf)\n",
                                    args->cam_addr.c_str(), i,
                                    mean.x, mean.y, mean.z,
                                    coordinate.latitude, coordinate.longitude);
                if (test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)) {
                    PRINTF_THREAD_STAMP("Catch termination flag!\n");
                    pthread_exit(nullptr);
                }
                args->hostProg->RoadBlockCoordinates.enqueue(coordinate);
            }
        } catch (std::invalid_argument &e) {
            fail_batch_num += 1.0;
            PRINTF_ERR_STAMP("%s\n", e.what());
        }
    } while (!(test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag)));
    PRINTF_THREAD_STAMP("Catch termination flag!\n");
    pthread_exit(nullptr);
}

void *RBDetectionMockThread(void *vargp) {
    auto args = (RBDetectionThreadArgs_t *) vargp;
    while (!(test_cancel(&args->hostProg->term_mutex, &args->hostProg->term_flag))) {
        PRINTF_THREAD_STAMP("Mock add RBCoordinate into queue\n");
        args->hostProg->RoadBlockCoordinates.enqueue(RBCoordinate(0, 0));
        sleep(1);
    }
    PRINTF_THREAD_STAMP("Catch termination flag, exit thread\n");
}
