#ifndef LOG_H
#define LOG_H

#include <stdio.h>


#ifdef DEBUG
    #define debug(fmt, ...) printf(fmt, ## __VA_ARGS__)
#else
    #define debug(fmt, ...)
#endif

#ifdef TEST
    #define info(fmt, ...)
    #define error(fmt, ...)
#else
    #define info(fmt, ...) printf(fmt, ## __VA_ARGS__)
    #define error(fmt, ...) printf(fmt, ## __VA_ARGS__)
#endif

#endif /* LOG_H */
