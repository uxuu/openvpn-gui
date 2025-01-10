
#ifndef RING_H
#define RING_H

#ifndef RING_NODE_SIZE
#define RING_NODE_SIZE  256
#endif

typedef struct _ring
{
    char (*ring)[RING_NODE_SIZE];
    size_t head;
    size_t tail;
    size_t next;
    size_t size;
} ring_t;

#define RING_INIT(xring, xsize)   do { \
        (xring).ring = (char (*)[RING_NODE_SIZE])malloc((xsize) * RING_NODE_SIZE); \
        if ((xring).ring != NULL) { \
            (xring).head = 0; \
            (xring).tail = 0; \
            (xring).next = 0; \
            (xring).size = (xsize); \
        } \
    } while(0)

#ifdef _MSC_VER
#if _WIN64
#define RING_ROUND(xsize) (1LL << (64 - __lzcnt64(xsize)))
#else
#define RING_ROUND(xsize) (1 << (32 - __lzcnt(xsize)))
#endif
#else
#if __x86_64__
#define RING_ROUND(xsize) (1 << (64 - __builtin_clzll((xsize))))
#else
#define RING_ROUND(xsize) (1 << (32 - __builtin_clz((xsize))))
#endif
#endif

#define RING_REINIT(xring, xsize)   do { \
        (xring).ring = (char (*)[RING_NODE_SIZE])realloc((xring).ring, RING_ROUND(xsize) * RING_NODE_SIZE); \
        if ((xring).ring != NULL) { \
            (xring).size = RING_ROUND(xsize); \
        } \
    } while(0)

#define RING_FREE(xring)   do { \
        free((xring).ring); \
        (xring).ring = NULL; \
    } while(0)

#define RING_NEED(xsize)   (((xsize) + RING_NODE_SIZE -1) / RING_NODE_SIZE)

#define RING_NEXT(xring, xsize)    do { \
        if ((xring).next + (xsize) > (xring).size) \
        { \
            (xring).head = 0; \
        } else {\
            (xring).head = (xring).next; \
        } \
        (xring).next += (xsize); \
    } while(0)


#endif /* RING_H */
