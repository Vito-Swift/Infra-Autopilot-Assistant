//
// Created by Vito Wu on 2021/6/17.
//

#include "utils.hh"

/* convert timeval to miliseconds */
#define TIMEVAL2F(stamp) \
    ((stamp).tv_sec * 1000.0 + (stamp).tv_usec / 1000.0)

/* get timestamp to the precision of miliseconds since the program starts */
double get_timestamp() {
    static double __init_stamp = -1;
    static struct timeval __cur_time;

    if (-1 == __init_stamp) {
        gettimeofday(&__cur_time, NULL);
        __init_stamp = TIMEVAL2F(__cur_time);
    }

    gettimeofday(&__cur_time, NULL);
    return ((TIMEVAL2F(__cur_time) - __init_stamp) / 1000.0);
}

#undef TIMEVAL2F

/* function: safe_malloc
 * usage: abort if malloc failed
 * arguments: size, number of bytes to allocate
 * return: a void* pointer
 */
void * safe_malloc(size_t size) {
    void *ptr = NULL;
    if (NULL == (ptr = malloc(size))) {
        EXIT_WITH_MSG("[!] insufficient memory\n");
    }
    return ptr;
}