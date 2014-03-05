#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include "rbuf.h"

uint32_t ring_buffer[RBUF_HIGH];
uint32_t local_rbuf_idx = 0;

/* Setting for 8 sub-buffers */
volatile uint32_t nway_ctrl[(1 << NUM_CHUNK)] = {0, 0, 0, 0};  

/* variables local to primary */
static uint8_t local_ctrl_idx = 0;
static uint32_t local_base = 0;

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

//setting CPU affinity
#ifdef __linux__
    cpu_set_t mask;
    CPU_ZERO(&mask);

    CPU_SET(0, &mask);
    // CPU_SET(2, &mask);
    // CPU_SET(4, &mask);
    // CPU_SET(6, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }
#elif defined(__osx__)
    thread_affinity_policy_data_t policy;
	policy.affinity_tag = 0;
	thread_policy_set(mach_thread_self(),
			  THREAD_AFFINITY_POLICY,
			  (thread_policy_t)&policy,
			  THREAD_AFFINITY_POLICY_COUNT);
#endif
    while (1) {
        for (int i=0; i < CHK_INTERVAL ; i++) {
            ENQ(100);
            INC(local_rbuf_idx, 1);
        }
        reset_rbuf_idx_no_worker();
        if (cond) break;
    }
    //signal worker to get out.
    ENQ(RBUF_EXIT);
    INC(local_rbuf_idx, 1);
    ENQ(RBUF_EXIT);
    INC(local_rbuf_idx, 1);
}
