/**
 * @filename: utils.hh
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/26/2021
 */

#ifndef LAMPPOSTAUTOCARDEMO_UTILS_HH
#define LAMPPOSTAUTOCARDEMO_UTILS_HH

#define MOCK

/*  ********************************************************
 *                 Included Headers and Macros
 * *********************************************************/

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <assert.h>
#include <string>
#include <regex>
#include <fcntl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/*  ********************************************************
 *                 Definitions
 * *********************************************************/

// default values for Backbone BATS network configuration
#define BACKBONE_PACKET_SIZE 3000UL
#define BACKBONE_SEND_PORT_DEFAULT 201
#define BACKBONE_RECV_PORT_DEFAULT 200
#define BACKBONE_SEND_INTERVAL 100UL
#define BACKBONE_ALIVE_INTERVAL 20UL

// default values for Hook tcp network configuration
#define HOOK_TCP_PORT 1020
#define HOOK_MAX_PACKET_SIZE 100000UL
#define HOOK_CONN_RETRY_INTERVAL 1
#define HOOK_MAX_COORD_NUM 20UL
#define HOOK_PACKET_INTERVAL 1

// defines for road blocks detections
#define RB_SEGMENT_THRESHOLD 1
#define RB_DEFAULT_REF_LONG (1.0)
#define RB_DEFAULT_REF_LATI (0.0)
#define RB_SAFE_BOUNDING (1UL)

// defines for path searching algorithm
const float step_size = 0.1;    // for each single step, the robot moves 0.1 (meter)

// default values for Zigbee network configuration
#define ZIGBEE_CTRL_PAN 1
#define ZIGBEE_CTRL_ADDR 50
#define ZIGBEE_ROOT_PAN 1
#define ZIGBEE_ROOT_ADDR 51

// default values for lmpctl subprogram
#define LMPCTL_STAT_FLAG 1
#define LMPCTL_LOG_FLAG 2
#define LMPCTL_SHUTDOWN_FLAG 3
const std::string lmpctl_fifo_name = "/tmp/lmpctl_fifo";

// default values for control channel
#define CONTROL_SHUTDOWN_FLAG (LMPCTL_SHUTDOWN_FLAG)

/*  ********************************************************
 *                 Data Structures
 * *********************************************************/

/**
 * Type: RBCoordinate
 * Note: This structure is set to store the coordinates of road blocks.
 *      In this structure,
 */
typedef struct RBCoordinate {
    double x;
    double y;

    RBCoordinate(double gps_x, double gps_y) : x(gps_x), y(gps_y) {}

    RBCoordinate() : x(0), y(0) {}
} RBCoordinate;

typedef struct LamppostBackbonePacket {
    RBCoordinate coord;
    uint32_t src_addr;
    bool _terminate;
} LamppostBackbonePacket_t;

typedef struct HookPacket {
    // RB Coordinates
    RBCoordinate coords[HOOK_MAX_COORD_NUM];
    int coords_num;

    // Zigbee Parameters
    int ctrl_zigbee_pan;
    int ctrl_zigbee_addr;
    int root_zigbee_pan;
    int root_zigbee_addr;

    uint32_t flag;
} HookPacket_t;

/**
 * Class: Queue
 * Note: This class is used as a thread-safe queue to mediate between multiple
 *      concurrent threads.
 * @tparam T: type of data element
 */
template<class T>
class Queue {
public:
    Queue() : q(), m(), c() { is_close = false; }

    ~Queue() {}

    void enqueue(T t) {
        std::lock_guard<std::mutex> lock(m);
        if (is_close)
            throw std::runtime_error("try to push into a closed queue!");
        q.push(t);
        c.notify_one();
    }

    bool dequeue(T &t) {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty() && !is_close) {
            c.wait(lock);
        }
        if (is_close)
            return false;
        t = q.front();
        q.pop();
        return true;
    }

    void close() noexcept {
        std::lock_guard<std::mutex> lock(m);
        is_close = true;
        c.notify_all();
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
    bool is_close;
};

template<class T>
class Vector {
public:
    Vector() : v(), m(), c() {}

    ~Vector() {}

    void push_back(T t) {
        std::lock_guard<std::mutex> lock(m);
        v.push_back(t);
        c.notify_one();
    }

    T &operator[](int idx) {
        std::lock_guard<std::mutex> lock(m);
        T &_ = v[idx];
        c.notify_one();
        return _;
    }

    int size() {
        return v.size();
    }

private:
    std::vector<T> v;
    mutable std::mutex m;
    std::condition_variable c;
};

/*  ********************************************************
 *                 Misc Utilities
 * *********************************************************/
#define TIMEVAL2F(stamp) \
    ((stamp).tv_sec * 1000.0 + (stamp).tv_usec / 1000.0)

double get_timestamp();

/* print msg with timestamp */
#define PRINTF_STAMP(format, ...) \
    do { \
        flockfile(stdout); \
        printf("%12.2f - ", get_timestamp()); \
        printf(format, ##__VA_ARGS__); \
        fflush(stdout); \
        funlockfile(stdout); \
    } while(0)

#define PRINTF_THREAD_STAMP(format, ...) \
    do { \
        flockfile(stdout); \
        printf("%12.2f - ", get_timestamp()); \
        printf("%s: \t", __FUNCTION__);     \
        printf(format, ##__VA_ARGS__); \
        fflush(stdout); \
        funlockfile(stdout); \
    } while(0)

/* print error msg to stderr */
#define PRINTF_ERR(format, ...) \
    do { \
        flockfile(stderr); \
        fprintf(stderr, format, ##__VA_ARGS__); \
        fflush(stderr); \
        funlockfile(stderr); \
    } while(0)

/* print error msg with timestamp to stderr */
#define PRINTF_ERR_STAMP(format, ...) \
    do { \
        flockfile(stderr); \
        fprintf(stderr, "%12.2f - ", get_timestamp()); \
        PRINTF_ERR(format, ##__VA_ARGS__); \
        funlockfile(stderr); \
    } while(0)

/* print error msg with timestamp to stderr then exit */
#define EXIT_WITH_MSG(format, ...) \
    do { \
        PRINTF_ERR_STAMP(format, ##__VA_ARGS__); \
        exit(-1); \
    } while (0)

/* print msg with timestamp to stderr if in debug mode */
#ifndef NDEBUG
#define PRINTF_DEBUG(format, ...) \
        PRINTF_ERR_STAMP(format, ##__VA_ARGS__)
#else
#define PRINTF_DEBUG(...)
#endif

#ifndef PRINTF_DEBUG_VERBOSE
#define PRINTF_DEBUG_VERBOSE(_verbose, format, ...) \
    do { \
        if (_verbose) \
            PRINTF_ERR_STAMP(format, ##__VA_ARGS__); \
    } while (0)
#endif

/* print array to stderr if in debug mode */
#ifndef NDEBUG
#define PRINT_ARRAY_DEBUG(ele_format, array, size) \
        do { \
            unsigned int i; \
            fprintf(stderr, "%12.2f - array " #array ": ", get_timestamp()); \
            for(i = 0; i < (size); i++) { \
                fprintf(stderr, ele_format, (array)[i]); \
            } \
            fprintf(stderr, "\n"); \
            fflush(stderr); \
        } while (0)
#else
#define PRINT_ARRAY_DEBUG(...)
#endif

/* function: safe_malloc
 * usage: abort if malloc failed
 * arguments: size, number of bytes to allocate
 * return: a void* pointer
 */
void *safe_malloc(size_t size);

/* macro: SMALLOC
 * usage: smart malloc, abort if malloc failed
 * arguments:
 *      1) type: type of the elements
 *      2) num: number of the elements
 * return: a type* pointer
 */
#define SMALLOC(type, num) \
    ((type*) safe_malloc((num) * sizeof(type)))

/* macro: SFREE
 * usage: smart free, release the address then set the pointer to NULL
 * arguments: a pointer whose memory chunk is to be released
 * return void
 */
#define SFREE(ptr) \
    do { \
        if(NULL != ptr) { \
            free(ptr); \
            ptr = NULL; \
        } \
    } while (0)


inline bool isFileExist(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

inline constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
}

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else

#include <unistd.h>

#define GetCurrentDir getcwd
#endif

inline int currentPath(std::string &path) {
    char cCurrentPath[FILENAME_MAX];

    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
        return errno;
    }

    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    path = std::string(cCurrentPath);
}

inline uint32_t parseNetAddrStr(const std::string &netAddrStr) {
    uint addr_0, addr_1, addr_2, addr_3;
    uint32_t addr = 0;
    if (sscanf(netAddrStr.c_str(), "%d.%d.%d.%d", &addr_0, &addr_1, &addr_2, &addr_3) != 4)
        return 0;
    addr = addr_0 << 24;
    addr |= addr_1 << 16;
    addr |= addr_2 << 8;
    addr |= addr_3;
    return addr;
}

template<typename T>
inline T
GetPropertyTree(const boost::property_tree::ptree &pt, const std::string entry, bool is_required, T *default_val) {
    if (!pt.get_optional<T>(entry).is_initialized()) {
        // Entry does not exist in property tree
        if (is_required) {
            EXIT_WITH_MSG("Get entry from configuration file error: %s is required but does not exist.\n",
                          entry.c_str());
        }
        if (default_val == nullptr) {
            EXIT_WITH_MSG("Failed to complete non-exist entry %s with default value.\n", entry.c_str());
        } else {
            PRINTF_STAMP("\t\tEntry %s does not exist, complete with default value.\n", entry.c_str());
            return *default_val;
        }
    } else {
        return pt.get<T>(entry);
    }
}

template<typename T>
inline T
GetPropertyTree(const boost::property_tree::ptree &pt, const std::string entry, bool is_required, T default_val) {
    if (!pt.get_optional<T>(entry).is_initialized()) {
        if (is_required) {
            EXIT_WITH_MSG("Get entry from configuration file error: %s is required but does not exist.\n",
                          entry.c_str());
        }
        return default_val;
    } else {
        return pt.get<T>(entry);
    }
}

inline bool SetSocketBlockingEnabled(int fd, bool blocking) {
    if (fd < 0) return false;

#ifdef _WIN32
    unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}

inline bool test_cancel(pthread_mutex_t *mutex, bool *val) {
    bool ret;
    pthread_mutex_lock(mutex);
    ret = *val;
    pthread_mutex_unlock(mutex);
    return ret;
}

#endif //LAMPPOSTAUTOCARDEMO_UTILS_HH
