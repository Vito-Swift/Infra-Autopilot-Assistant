/**
 * @filename: LP_General.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/27/2021
 */

#include "LPHost/LamppostHostGeneral.hh"

std::atomic<bool> term_flag;
LamppostHostProg *globalHostProg;

void interrupt_handler(int dummy) {
    PRINTF_STAMP("Catch CTRL-C signal.\n");
    PRINTF_STAMP("Terminate detached threads and exit.\n");
    term_flag = true;
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

    if (options->is_root_node) {
        // TODO: Parse Robot Section
        try {

        } catch (const std::invalid_argument &e) {
            exit(1);
        } catch (const std::out_of_range &e) {
            exit(1);
        }

        std::string default_hook_addr("192.168.1.1"); // The default IP address of RaspberryPI
        int default_hook_port = HOOK_TCP_PORT;
        options->hook_ip_addr = GetPropertyTree<std::string>(pt, "Hook.HookAddr", false, &default_hook_addr);
        options->hook_ip_port = GetPropertyTree<int>(pt, "Hook.HookPort", false, &default_hook_port);
        options->ctrl_zigbee_addr = GetPropertyTree<int>(pt, "Hook.CtrlZigbeeAddr", true, nullptr);
        options->ctrl_zigbee_pan = GetPropertyTree<int>(pt, "Hook.CtrlZigbeePan", true, nullptr);
        options->root_zigbee_addr = GetPropertyTree<int>(pt, "Hook.RootZigbeeAddr", true, nullptr);
        options->root_zigbee_pan = GetPropertyTree<int>(pt, "Hook.RootZigbeePan", true, nullptr);
    }
}

void print_option_setting(Options *options) {
    PRINTF_STAMP("Print options from cmd arguments and configuration file:\n");
    PRINTF_STAMP("\t\tOption - mock-detection: %s\n", options->mock_detection ? "true" : "false");
    PRINTF_STAMP("\t\tLocal BATS Addr: %s\n", options->netaddr_str.c_str());
    PRINTF_STAMP("\t\tRoot BATS Addr: %s\n", options->root_net_addr.c_str());
    PRINTF_STAMP("\t\tRoot BATS Port: %d\n", options->root_port);
    PRINTF_STAMP("\t\tRoot Node: %s\n", options->is_root_node ? "true" : "false");
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
    term_flag = false;
    globalHostProg = lamppostProg;
}

void lamppost_program_run(LamppostHostProg *lamppostProg) {
    signal(SIGINT, interrupt_handler);

    // launch thread to detect from video stream
    RBDetectionThreadArgs_t detection_args{.hostProg = lamppostProg, .terminate_flag = &term_flag};
    if (lamppostProg->options.mock_detection) {
        PRINTF_STAMP("Launch mock thread to detect road blocks...\n");
        pthread_create(&lamppostProg->detection_thread, nullptr, RBDetectionMockThread, (void *) &detection_args);
    } else {
        PRINTF_STAMP("Launch thread to detect road blocks...\n");
        pthread_create(&lamppostProg->detection_thread, nullptr, RBDetectionThread, (void *) &detection_args);
    }
    pthread_detach(lamppostProg->detection_thread);

    // launch thread to send coordinates of detected road block to root node
    PRINTF_STAMP("Launch thread to communicate with root node...\n");
    SendThreadArgs_t sendThread_args{.hostProg = lamppostProg, .terminate_flag = &term_flag};
    pthread_create(&lamppostProg->send_thread, nullptr, LamppostHostSendThread, (void *) &sendThread_args);
    pthread_detach(lamppostProg->send_thread);

    // if the current node is root node, launch thread to manage received data from other nodes
    if (lamppostProg->options.is_root_node) {
        PRINTF_STAMP("Launch thread to manage received data from slave nodes");
        RecvThreadArgs_t recvThread_args{.hostProg = lamppostProg, .terminate_flag = &term_flag};
        pthread_create(&lamppostProg->recv_thread, nullptr, LamppostHostRecvThread, (void *) &recvThread_args);
        pthread_detach(lamppostProg->recv_thread);
    }

    if (lamppostProg->options.is_root_node) {
        PRINTF_STAMP("Launch thread to communicate with hook node as option.is_root_node is enabled...\n");
        HookSendThreadArgs_t hookSendThreadArgs{.hostProg=lamppostProg, .terminate_flag=&term_flag};
        pthread_create(&lamppostProg->hook_thread, nullptr, LamppostHostCommHookSendThread, (void *) &sendThread_args);
        pthread_detach(lamppostProg->hook_thread);
    }

    while (!term_flag) {
        // determine whether the program needs to terminate
        PRINTF_STAMP("Main program is still alive...\n");
        sleep(10);
    }
}

void lamppost_program_exit(LamppostHostProg *lamppostProg) {
    PRINTF_STAMP("Clean and exit.\n");
    options_free(&lamppostProg->options);
}
