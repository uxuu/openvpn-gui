
#ifndef MEMPOOL_H
#define MEMPOOL_H

#ifndef MEMPOOL_NODE_SIZE
#define MEMPOOL_NODE_SIZE  256
#endif

#include <intrin.h>

#ifdef _WIN64
#define BSR  _BitScanReverse64
#else
#define BSR  _BitScanReverse
#endif

typedef struct _mempool
{
    char (*ring)[MEMPOOL_NODE_SIZE];
    size_t head;
    size_t tail;
    size_t next;
    size_t size;
} mempool_t;

#define MEMPOOL_INIT(_pool, _size)   do { \
        (_pool).ring = (char (*)[MEMPOOL_NODE_SIZE])malloc((_size) * MEMPOOL_NODE_SIZE); \
        if ((_pool).ring != NULL) { \
            (_pool).head = 0; \
            (_pool).tail = 0; \
            (_pool).next = 0; \
            (_pool).size = (_size); \
        } \
    } while(0)


#define MEMPOOL_REINIT(_pool, _size)   do { \
        int __idx = 0; \
        BSR(&__idx, (_size)); \
        __idx ++; \
        (_pool).ring = (char (*)[MEMPOOL_NODE_SIZE])realloc((_pool).ring, (1LL << __idx) * MEMPOOL_NODE_SIZE); \
        if ((_pool).ring != NULL) { \
            (_pool).size = (1LL << __idx) ; \
        } \
    } while(0)

#define MEMPOOL_FREE(_pool)   do { \
        free((_pool).ring); \
        (_pool).ring = NULL; \
    } while(0)

#define MEMPOOL_NEED(_size)   (((_size) + MEMPOOL_NODE_SIZE -1) / MEMPOOL_NODE_SIZE)

#define MEMPOOL_NEXT(_pool, _size)    do { \
        if ((_pool).next + (_size) > (_pool).size) \
        { \
            (_pool).head = 0; \
        } else {\
            (_pool).head = (_pool).next; \
        } \
        (_pool).next += (_size); \
    } while(0)


#endif /* MEMPOOL_H */
