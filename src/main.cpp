#include <stdio.h>
#include <stdint.h>

#ifdef __linux
#include <sched.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif
#include <unistd.h>
#include "rbuf.h"

#define CHK_INTERVAL 128
#define MAX_RUN 8 * 1024 * 1024 * 1024

uint32_t ring_buffer[RBUF_HIGH];
uint32_t local_rbuf_idx = 0;

/* Setting for 8 sub-buffers */
volatile uint32_t nway_ctrl[(1 << NUM_CHUNK)] = {0, 0, 0, 0};  

/* variables local to primary */
static uint8_t local_ctrl_idx = 0;
static uint32_t local_base = 0;

/* execution mode */
enum exec_mode {STANDALONE, WORKER};

static bool
inline rbuf_overflow()
{
  return (local_rbuf_idx - local_base) > NWAY_BOUND;
}

static void
inline reset_rbuf_idx()
{

  //adding control command
  ring_buffer[local_rbuf_idx++] = RBUF_WRAP;

  // update ctrl_array -- access to shared variable
  nway_ctrl[local_ctrl_idx] = local_rbuf_idx - local_base;

  //update base
  local_base =  local_rbuf_idx;

  // move local index
  local_ctrl_idx = ((local_ctrl_idx + 1) & ((1 << NUM_CHUNK) - 1));

  // polling for progress -- access to shared variable
  while (nway_ctrl[local_ctrl_idx] != 0) {
    usleep(50);
  }

  //resetting local idx
  if (local_ctrl_idx == 0) {
    local_rbuf_idx = 0;
    local_base = 0;
  }
}

static void
inline reset_rbuf_idx_no_worker()
{
  //adding control command
  ring_buffer[local_rbuf_idx++] = RBUF_WRAP;

  // update ctrl_array -- access to shared variable
  nway_ctrl[local_ctrl_idx] = local_rbuf_idx - local_base;

  //local update
  local_base =  local_rbuf_idx;

  // move local index
  local_ctrl_idx = ((local_ctrl_idx + 1) & ((1 << NUM_CHUNK) - 1));

  //resetting local idx
  if (local_ctrl_idx == 0) {
    local_rbuf_idx = 0;
    local_base = 0;
  }
}

int main (int argc, char* argv[]){
exec_mode mode = STANDALONE;
bool exit_flag = false;
uint64_t counter = 0;

//setting CPU affinity
#ifdef __linux
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }
#elif defined(__APPLE__)
    thread_affinity_policy_data_t policy;
	policy.affinity_tag = AFFINITY_TAG;
	thread_policy_set(mach_thread_self(),
			  THREAD_AFFINITY_POLICY,
			  (thread_policy_t) &policy,
			  THREAD_AFFINITY_POLICY_COUNT);
#endif

    if (mode != STANDALONE) {
        //fork thread here.
    }

    while (1) {
        for (int i=0; i < CHK_INTERVAL ; i++) {
            ENQ(local_rbuf_idx, 100);
            INC(local_rbuf_idx, 1);

            if (++counter > (uint64_t) MAX_RUN)
                exit_flag = true;
        }
        if (rbuf_overflow()) {
            if (mode == STANDALONE) {
               reset_rbuf_idx_no_worker();
            } else {
                reset_rbuf_idx();
            }
        }
        if (exit_flag) break;
    }
    //signal worker to get out.
    ENQ(local_rbuf_idx, RBUF_EXIT);
    INC(local_rbuf_idx, 1);
    //double checking.
    ENQ(local_rbuf_idx, RBUF_EXIT);
    INC(local_rbuf_idx, 1);
}
