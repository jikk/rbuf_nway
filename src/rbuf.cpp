#include <fcntl.h>
#include <stdint.h>
#include <sys/mmap.h>
#include <sys/stat.h>

#include "rbuf.h"


/* 
 * __CLONE__ derective is defined when the secondary is forked via clone() 
 * call. 
 * In this case, the primary and the secondary will share the same address 
 * space.
 */


/* #define __CLONE__ */

#if defined __CLONE__

uint32_t rbuf[BUF_SIZE];
uint32_t pidx = 0;
uint32_t sidx = 0;

/* nothing to do here */
void rbuf_alloc(void) {
  ;
}

#else

/* global variables */
uint32_t *rbuf;
uint32_t *pidx;
uint32_t *sidx;

void
rbuf_alloc(void) {

  uint32_t *rbuf_;

  /* allocate shared object */
  int32_t fd = shm_open(SHM_NAME,
                 O_CREAT | O_EXCL | O_RDWR,
                 S_IRWXU | S_IRWXG);

  if (fd < 0) {
    perror("err:shm_open");
    shm_unlink(SHM_NAME);
    exit(-1);
  }

  /* ftruncate */
  if (ftruncate(fd,
        (BUF_SIZE + 2) * sizeof(uint32_t))
      == -1)
  {
    perror("err:ftruncate");
    exit(-1);
  }

  /* buffer allocation */
  rbuf_ = (uint32_t *) mmap(NULL,
           (BUF_SIZE + 2) * sizeof(uint32_t),
           PROT_READ|PROT_WRITE,
           MAP_SHARED,
           fd, 0)

  if (rbuf_ == MAP_FAILED) {
    perror("err:mmap");
    exit(-1);
  }
  pidx = &rbuf_[0];
  sidx = &rbuf_[1];
  rbuf = &rbuf_[2];
}

#endif
