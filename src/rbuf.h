#ifndef __RBUF_H__
#define __RBUF_H__
/*
 * Ring buffer size
 */

#define RBUF_FACTOR 4

/* kernel ending address for 32bit address space */
#define RBUF_FULL 0xFFFFFFFFU

/* RBUF size choices */

// #define RBUF_SIZE 0x3FFF  * RBUF_FACTOR    /* 16KB * 4 * 4 bytes    */
// #define RBUF_SIZE 0x7FFF  * RBUF_FACTOR    /* 32KB * 4 * 4 bytes    */
// #define RBUF_SIZE 0xFFFF  * RBUF_FACTOR    /* 64KB * 4 * 4 bytes    */
// #define RBUF_SIZE 0x1FFFF * RBUF_FACTOR    /* 128KB * 4 * 4 bytes   */
// #define RBUF_SIZE 0x3FFFF * RBUF_FACTOR    /* 256KB * 4 * 4 bytes   */
// #define RBUF_SIZE 0x7FFFF * RBUF_FACTOR    /* 512KB * 4 * 4 bytes   */
// #define RBUF_SIZE 0xFFFFF * RBUF_FACTOR    /* 1024KB * 4 * 4 bytes  */

#define RBUF_SIZE 0x7FFF  * RBUF_FACTOR    /* 32KB * 4 * 4 bytes    */

#define RBUF_HIGH RBUF_SIZE * 2

/* Control cmds */
#define NUM_CTRL_CMD 4  /* number of ctrl command  */
#define RBUF_WRAP    RBUF_FULL - 2   /* 0xfffffffd */
#define INIT_STATE   RBUF_FULL - 3
#define FINI_STATE   RBUF_FULL - 4
#define RBUF_EXIT    FINI_STATE

/* N-way buffer setting */
#define NUM_CHUNK 2
#define NWAY_BOUND (RBUF_SIZE >> NUM_CHUNK)

/* MACROs for ENQ and INC operation */
#define ENQ(idx, ea) ring_buffer[(idx)] = ea

/* LEA instruction used not to pollute eflag register */
#define INC(idx, num)             \
  __asm__ (                       \
  "lea 1(%1), %0\n"               \
  : "=r" (idx)                    \
  : "r" (idx)                     \
)

#endif /* __RBUF_H__ */
