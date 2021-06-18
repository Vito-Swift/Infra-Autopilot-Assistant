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

/*  ********************************************************
 *                 Data Structures
 * *********************************************************/

/**
 * Type: RBCoordinate
 * Note: This structure is set to store the coordinates of road blocks.
 *      In this structure,
 */
typedef struct {
    uint32_t index;             // index of the (optional)
    double gps_x;               //
    double gps_y;
} RBCoordinate;

/**
 * Class: Queue
 * Note: This class is used as a thread-safe queue to mediate between multiple
 *      concurrent threads.
 * @tparam T: type of data element
 */
template<class T>
class Queue {
public:
    Queue() : q(), m(), c() {}

    ~Queue() {}

    void enqueue(T t) {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    T dequeue(void) {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty()) {
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

private:
    std::queue<T> q;
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

#endif //LAMPPOSTAUTOCARDEMO_UTILS_HH
