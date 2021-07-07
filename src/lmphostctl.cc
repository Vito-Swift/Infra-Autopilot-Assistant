//
// Created by Vito Wu on 2021/7/2.
//

#include <iostream>
#include <boost/program_options.hpp>
#include <exception>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.hh"

using std::cout;
using std::cerr;
using std::endl;
using std::exception;

int main(int argc, char **argv) {
    boost::program_options::options_description desc("Lamppost Host Controller");
    auto config_option = boost::program_options::value<std::string>();
    config_option->value_name("FILE");
    config_option->default_value("");
    desc.add_options()
            ("help,h", "show this message")
            ("service,c", boost::program_options::value<std::string>()->value_name("COMMAND"), "[start|stop|stat|log]")
            ("config,c", config_option, "must specify if -service start is invoked");

    try {
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("help") || argc == 1) {
            cout << desc << std::endl;
            exit(0);
        }

        if (vm.count("service")) {
            std::string command = vm["service"].as<std::string>();

            if (command == "start") {
                // launch the system
                std::string config_file = vm["config"].as<std::string>();
                if (config_file.empty()) {
                    cerr << "error: must provide config file (*.ini) by option --config to start the program"
                         << std::endl;
                    return 1;
                }

            } else if (command == "stop") {
                // shutdown the system

            } else if (command == "stat") {
                // show statistic message

            } else if (command == "log") {
                // show log message

            } else {
                // undefined behavior

                return 1;
            }
        }

    } catch (exception &e) {
        cerr << "error: " << e.what() << std::endl;
        cout << desc << std::endl;
        return 1;
    } catch (...) {
        cerr << "Exception of unknown type!\n" << std::endl;
        return 1;
    }
    return 0;
}