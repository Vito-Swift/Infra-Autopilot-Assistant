/**
 * @filename: LP_General.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#include "LPHost/LamppostHostGeneral.hh"

LamppostHostProg *globalHostProg;

void interrupt_handler(int dummy) {
    PRINTF_STAMP("Catch CTRL-C signal.\n");
    PRINTF_STAMP("Terminate detached threads and exit.\n");
    pthread_mutex_lock(&globalHostProg->term_mutex);
    globalHostProg->term_flag = true;
    pthread_mutex_unlock(&globalHostProg->term_mutex);
    lamppost_program_exit(globalHostProg);
    sleep(5);
    pthread_exit(nullptr);
}

static inline void copy_opt(char **str, char *optarg) {
    if (NULL == ((*str) = strndup(optarg, 1024))) {
        PRINTF_ERR("[!] invalid input parameter\n");
    }
}

void options_init(Options *options) {
    options->config_file_path = nullptr;
    options->is_root_node = false;
}

bool options_validate(Options *options) {
    if (options->config_file_path == nullptr) {
        EXIT_WITH_MSG("Options.config_file_path gets NULL, please check input argument. Exit.\n");
    }

    if (!isFileExist(options->config_file_path)) {
        EXIT_WITH_MSG("File not exist: %s. Exit.\n", options->config_file_path);
    }
    return true;
}

void parse_configuration_file(Options *options) {
    PRINTF_STAMP("Reading configuration file: %s\n", options->config_file_path);
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(options->config_file_path, pt);

    // Parse Networking Section
    try {
        options->netaddr_str = GetPropertyTree<std::string>(pt, "Networking.LocalBATSAddr", true, nullptr);
        options->root_net_addr = GetPropertyTree<std::string>(pt, "Networking.RootBATSAddr", true, nullptr);
        options->root_port = GetPropertyTree<int>(pt, "Networking.RootBATSPort", true, nullptr);
        bool is_root_node_def = false;
        options->is_root_node = GetPropertyTree<bool>(pt, "Networking.IsRootNode", false, &is_root_node_def);
    } catch (const std::invalid_argument &e) {
        PRINTF_ERR_STAMP("Parse networking section get invalid_argument failed: %s\n", e.what());
        exit(1);
    } catch (const std::out_of_range &e) {
        PRINTF_ERR_STAMP("Parse networking section get out_of_range error: %s\n", e.what());
        exit(1);
    }

    // Parse RBDetection Section
    if (!options->mock_detection) {
        try {
            options->cam1_addr = GetPropertyTree<std::string>(pt,
                                                              "RBDetection.Camera1",
                                                              false,
                                                              "udp://0.0.0.0:8000/");
            options->cam2_addr = GetPropertyTree<std::string>(pt,
                                                              "RBDetection.Camera2",
                                                              false,
                                                              "udp://0.0.0.0:8001/");
            options->marker_size = GetPropertyTree<float>(pt, "RBDetection.MarkerSize", false, 0.5);
            options->cam1_ref_id = GetPropertyTree<int>(pt, "RBDetection.Cam1RefMarker", false, 0);
            options->cam2_ref_id = GetPropertyTree<int>(pt, "RBDetection.Cam2RefMarker", false, 0);
            options->frame_increment = GetPropertyTree<int>(pt, "RBDetection.FrameIncrement", false, 100);
            options->calibration_file = GetPropertyTree<std::string>(pt, "RBDetection.CalibrationFile", true, nullptr);
            options->ref_gps_file = GetPropertyTree<std::string>(pt, "RBDetection.GPSReferenceFile", true, nullptr);

            // validate files
            if (!isFileExist(options->calibration_file)) {
                PRINTF_ERR_STAMP("Calibration file does not exist: %s\n", options->calibration_file.c_str());
                exit(1);
            }

            if (!isFileExist(options->ref_gps_file)) {
                PRINTF_ERR_STAMP("GPS reference file does not exist: %s\n", options->ref_gps_file.c_str());
                exit(1);
            }

        } catch (const std::invalid_argument &e) {
            PRINTF_ERR_STAMP("Parse RBDetection section get invalid_argument failed: %s\n", e.what());
            exit(1);
        } catch (const std::out_of_range &e) {
            PRINTF_ERR_STAMP("Parse RBDetection section get out_of_range error: %s\n", e.what());
            exit(1);
        }
    }

    if (options->is_root_node) {
        // TODO: Parse Robot Section
//        try {
//
//        } catch (const std::invalid_argument &e) {
//            exit(1);
//        } catch (const std::out_of_range &e) {
//            exit(1);
//        }

//        std::string default_hook_addr("192.168.1.1"); // The default IP address of RaspberryPI
//        int default_hook_port = HOOK_TCP_PORT;
//        options->hook_ip_addr = GetPropertyTree<std::string>(pt, "Hook.HookAddr", false, &default_hook_addr);
//        options->hook_ip_port = GetPropertyTree<int>(pt, "Hook.HookPort", false, &default_hook_port);
//        options->ctrl_zigbee_addr = GetPropertyTree<int>(pt, "Hook.CtrlZigbeeAddr", true, nullptr);
//        options->ctrl_zigbee_pan = GetPropertyTree<int>(pt, "Hook.CtrlZigbeePan", true, nullptr);
//        options->root_zigbee_addr = GetPropertyTree<int>(pt, "Hook.RootZigbeeAddr", true, nullptr);
//        options->root_zigbee_pan = GetPropertyTree<int>(pt, "Hook.RootZigbeePan", true, nullptr);
    }
}

void print_option_setting(Options *options) {
    PRINTF_STAMP("Print options from cmd arguments and configuration file:\n");
    PRINTF_STAMP("\t\tOption - mock-detection: %s\n", options->mock_detection ? "true" : "false");
    PRINTF_STAMP("\t\tLocal BATS Addr: %s\n", options->netaddr_str.c_str());
    PRINTF_STAMP("\t\tRoot BATS Addr: %s\n", options->root_net_addr.c_str());
    PRINTF_STAMP("\t\tRoot BATS Port: %d\n", options->root_port);
    PRINTF_STAMP("\t\tRoot Node: %s\n", options->is_root_node ? "true" : "false");
//    if (options->is_root_node) {
//        PRINTF_STAMP("\t\tHook addr: %s\t port: %d\n", options->hook_ip_addr.c_str(), options->hook_ip_port);
//        PRINTF_STAMP("\t\tControl node PAN: %d\t addr: %d\n", options->ctrl_zigbee_pan, options->ctrl_zigbee_addr);
//        PRINTF_STAMP("\t\tRoot node PAN: %d\t addr: %d\n", options->root_zigbee_pan, options->root_zigbee_addr);
//    }
    if (!options->mock_detection) {
        PRINTF_STAMP("\t\tCamera 1 addr: %s\n", options->cam1_addr.c_str());
        PRINTF_STAMP("\t\tCamera 2 addr: %s\n", options->cam2_addr.c_str());
        PRINTF_STAMP("\t\tArUco marker for reference: %d, %d\n", options->cam1_ref_id, options->cam2_ref_id);
        PRINTF_STAMP("\t\tArUco marker size: %lf\n", options->marker_size);
        PRINTF_STAMP("\t\tArUco detection frame increment: %d\n", options->frame_increment);
        PRINTF_STAMP("\t\tCamera calibration file: %s\n", options->calibration_file.c_str());
        PRINTF_STAMP("\t\tGPS reference file: %s\n", options->ref_gps_file.c_str());
    }

}

void options_parse(Options *options, int argc, char **argv) {
    int c, opt_idx;
    if (argc == 1) {
        print_usage(argv[0]);
        options_free(options);
        exit(0);
    }

    while (-1 != (c = getopt_long(argc, argv, "h", lamppost_host_long_opts, &opt_idx))) {
        switch (c) {
            case 0:
                if (lamppost_host_long_opts[opt_idx].flag == 0) {
                    PRINTF_STAMP("\toption %s: %s\n", lamppost_host_long_opts[opt_idx].name,
                                 optarg ? optarg : "null");
                }
                break;

            case 'h':
                print_usage(argv[0]);
                options_free(options);
                exit(0);

            case OP_CONFIG_PATH:
                copy_opt(&options->config_file_path, optarg);
                break;

            case OP_MOCK_DETECTION:
                options->mock_detection = true;

            case '?':
                break;

            default:
                EXIT_WITH_MSG("[!] unknown parameter, exit...\n");
        }
    }

    options_validate(options);
    parse_configuration_file(options);
    print_option_setting(options);
}

void options_free(Options *options) {
    if (options->config_file_path != nullptr)
        SFREE(options->config_file_path);
}

void lamppost_program_init(LamppostHostProg *lamppostProg, int argc, char **argv) {
    options_init(&lamppostProg->options);
    options_parse(&lamppostProg->options, argc, argv);
    pthread_mutex_lock(&lamppostProg->term_mutex);
    lamppostProg->term_flag = false;
    pthread_mutex_unlock(&lamppostProg->term_mutex);
    globalHostProg = lamppostProg;
}

void lamppost_program_run(LamppostHostProg *lamppostProg) {
    //signal(SIGINT, interrupt_handler);

    // launch thread to detect from video stream
    RBDetectionThreadArgs_t detect_thread_args1;
    RBDetectionThreadArgs_t detect_thread_args2;
    if (lamppostProg->options.mock_detection) {
        detect_thread_args1.hostProg = lamppostProg;
        PRINTF_STAMP("Launch mock thread to detect road blocks...\n");
        pthread_create(&lamppostProg->detection_thread1, nullptr, RBDetectionMockThread, (void *) &detect_thread_args1);
        pthread_detach(lamppostProg->detection_thread1);
    } else {
        PRINTF_STAMP("Launch thread to detect road blocks...\n");
        detect_thread_args1.hostProg = lamppostProg;
        detect_thread_args1.cam_addr = lamppostProg->options.cam1_addr;
        detect_thread_args1.ref_marker_id = lamppostProg->options.cam1_ref_id;
        pthread_create(&lamppostProg->detection_thread1, nullptr, RBDetectionThread, (void *) &detect_thread_args1);
        pthread_detach(lamppostProg->detection_thread1);
    }

    // launch thread to send coordinates of detected road block to root node
    PRINTF_STAMP("Launch thread to communicate with root node...\n");
    SendThreadArgs_t sendThread_args{.hostProg = lamppostProg};
    pthread_create(&lamppostProg->send_thread, nullptr, LamppostHostSendThread, (void *) &sendThread_args);
    pthread_detach(lamppostProg->send_thread);

    PRINTF_STAMP("Launch thread to manage received data from other nodes\n");
    RecvThreadArgs_t recvThread_args{.hostProg = lamppostProg};
    pthread_create(&lamppostProg->recv_thread, nullptr, LamppostHostRecvThread, (void *) &recvThread_args);
    pthread_detach(lamppostProg->recv_thread);


    if (lamppostProg->options.is_root_node) {
        //PRINTF_STAMP("Launch thread to communicate with hook node as option.is_root_node is enabled...\n");
        //HookSendThreadArgs_t hookSendThreadArgs{.hostProg=lamppostProg};
//        pthread_create(&lamppostProg->hook_thread,
//                       nullptr,
//                       LamppostHostCommHookSendThread,
//                       (void *) &sendThread_args);
//        pthread_detach(lamppostProg->hook_thread);

        PRINTF_STAMP("Launch thread to receive control message from lmpctl...\n");
        LmpCtlListenerArgs_t lmpCtlListenerArgs{.hostProg=lamppostProg};
        pthread_create(&lamppostProg->lmpctl_thread, nullptr, LamppostHostLmpCtlListenerThread,
                       (void *) &lmpCtlListenerArgs);
        pthread_detach(lamppostProg->lmpctl_thread);
    }

    while (!test_cancel(&(lamppostProg->term_mutex), &(lamppostProg->term_flag))) {
        // determine whether the program needs to terminate
        PRINTF_STAMP("Main program is still alive...\n");

        if (lamppostProg->options.is_root_node) {
            pthread_mutex_lock(&lamppostProg->lal_modify_mutex);
            time_t now = time(nullptr);
            for (int i = 0; i < lamppostProg->LamppostAliveList.size(); i++) {
                if (now - lamppostProg->LamppostAliveList[i].second >= BACKBONE_ALIVE_INTERVAL) {
                    PRINTF_THREAD_STAMP("Node %d has been disconnected for %d seconds, remove it from alive list\n",
                                        lamppostProg->LamppostAliveList[i].first, BACKBONE_ALIVE_INTERVAL);
                    lamppostProg->LamppostAliveList.erase(lamppostProg->LamppostAliveList.begin() + i);
                }
            }
            if (!lamppostProg->LamppostAliveList.empty()) {
                std::cout << "Alive lampposts: ";
                for (auto &i: lamppostProg->LamppostAliveList) {
                    std::cout << i.first << " ";
                }
                std::cout << std::endl;
            }

            pthread_mutex_unlock(&lamppostProg->lal_modify_mutex);
        }
        sleep(3);
    }
}

void lamppost_program_exit(LamppostHostProg *lamppostProg) {
    PRINTF_STAMP("Clean and exit.\n");
    options_free(&lamppostProg->options);
    lamppostProg->RoadBlockCoordinates.close();
    // wait for other thread to terminate gracefully
    sleep(5);
}
