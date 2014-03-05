#include <stdint.h>
#include "rbuf_nway.h"

/* Shared global variables */
extern uint32_t ring_buffer[RBUF_HIGH];
extern uint32_t local_rbuf_idx = 0;

void worker_main(void *args){
    uint32_t bufsize = 0;
        
    /* variables for local nway buffer control */
    uint8_t  local_ctrl_idx = 0;

    /* process pass-in variable and setup (a single) thread context */
    volatile uint32_t *nway_ctrl = (uint32_t*) args;

    uint32_t ret = 0;

    /* setting cpu affinity */
    cpu_set_t mask; 
    CPU_ZERO(&mask);
    CPU_SET(2, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }

    while(1){
        /* wait for the next chunk */
        while(nway_ctrl[local_ctrl_idx] == 0) {
            usleep(50);
        }
        bufsize = nway_ctrl[local_local_rbuf_idx];
        ret = batch_process(bufsize);

        nway_ctrl[local_ctrl_idx] = 0;
        local_ctrl_idx = ((local_ctrl_idx + 1) & ((1 << NUM_CHUNK) - 1));

        //resetting local index
        if (local_ctrl_idx == 0)
            local_rbuf_idx = 0;

        /* taking care of 'ret' value */
        if (ret == RBUF_EXIT)
            break;
    }   // while-loop
}

uint32_t batch_process(int32_t bufsize) {
    for (int count = 0;; local_rbuf_idx++) {
        if (ring_buffer[local_rbuf_idx] == RBUF_EXIT) 
            return RBUF_EXIT;
        else if (ring_buffer[local_rbuf_idx] == RBUF_WRAP) 
            if (count == bufsize) {
                ;
            } else {
                fprintf(stderr,"Error: count doesn't match %d %d \n", bufsize, count);
            }

            return RBUF_WRAP;
        else {
            //do something.
            count++;
        }
    }
}

void worker_null(void *args) {
    /* variables for local nway buffer control */
    uint8_t  local_ctrl_idx = 0;

    /* process pass-in variable and setup (a single) thread context */  
    volatile uint32_t *nway_ctrl = (uint32_t*) args;

    // setting cpu affinity
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(2, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
      perror("sched_setaffinity");
    }

    while(1) {
      /* wait for the next chunk */
      while(nway_ctrl[local_ctrl_idx] == 0) {
        usleep(50);
      }

      //bufsize = nway_ctrl[local_ctrl_idx];
      for (;; local_rbuf_idx++)
      {
        if (ring_buffer[local_rbuf_idx] == RBUF_EXIT) {
          return;
        } else if (ring_buffer[local_rbuf_idx] == RBUF_WRAP) {
          local_rbuf_idx++;
          break;
        }
      }  // for-loop

      nway_ctrl[local_ctrl_idx] = 0;
      local_ctrl_idx = ((local_ctrl_idx + 1) & ((1 << NUM_CHUNK) - 1));

      //resetting local index
      if (local_ctrl_idx == 0)
        local_rbuf_idx = 0;
    } // while-loop
}
