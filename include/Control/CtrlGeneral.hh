#ifndef LAMPPOSTAUTOCARDEMO_CTRLGENERAL_HH
#define LAMPPOSTAUTOCARDEMO_CTRLGENERAL_HH

#include "utils.hh"

namespace Control {

    typedef struct {
        float lt_point;
        float lb_point;
        float rt_point;
        float rb_point;
    } mapinfo_t;

    typedef struct {
        std::string robot_addr;
        int command_port;
        int video_port;

        // variables to store information of the global map
        std::vector <path> path;
        mapinfo_t mapinfo;
        bool is_path_planned;

        // variables to interrupt the execution
        bool is_shutdown;
        pthread_mutex_t shutdown_mutex;
    } MetaController_t;

    void generate_roadblock_map();

    void generate_path();

    void run();
}

#endif //LAMPPOSTAUTOCARDEMO_CTRLGENERAL_HH
