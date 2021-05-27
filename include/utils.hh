/**
 * @filename: utils.hh
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 5/26/2021
 */

#ifndef LAMPPOSTAUTOCARDEMO_UTILS_HH
#define LAMPPOSTAUTOCARDEMO_UTILS_HH

/*  ********************************************************
 *                 Included Headers and Macros
 * *********************************************************/

#include <condition_variable>
#include <mutex>
#include <queue>

/*  ********************************************************
 *                 Data Structures
 * *********************************************************/

/**
 * Type: Options
 * Note: This structure is set to store the user options passed when init.
 *      The corresponding usage of each command is written as comment after
 *      each data field.
 */
typedef struct {
    int pan;                    // network pan of localhost
    int addr;                   // network address of localhost
    int root_pan;               // network pan of root lamppost
    int root_addr;              // network address of root lamppost
    bool mock_detection;        // if this option is enabled, road block detection will be mocked
} Options;

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
 * Type: LamppostProg
 * Note: This structure contains all the intermediate meta-data essential to
 *      the whole execution flow. In practice, users passed this option to
 *      LamppostProg_init(), LamppostProg_exec(), LamppostProg_exit() in
 *      order to control the execution flow
 */
typedef struct {
    Options options;
    pthread_t detection_handler;
    pthread_t comm_handler;
    pthread_mutex_t mutex_RBC;
    std::vector <RBCoordinate> RoadBlockCoordinates;
} LamppostProg;

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

#endif //LAMPPOSTAUTOCARDEMO_UTILS_HH
