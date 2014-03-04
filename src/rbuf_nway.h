#ifndef __RBUF_NWAY_H__
#define __RBUF_NWAY_H__

#define NUM_CHUNK 3
#define NWAY_BOUND (RBUF_SIZE >> NUM_CHUNK)

typedef struct {
  volatile uint32_t *nway_ctrl;
} sec_arg_t;

#endif /* __RBUF_NWAY_H__ */
