#include <stdint.h>
#include "rbuf_nway.h"

uint32_t ring_buffer[RBUF_HIGH];
uint32_t local_rbuf_idx = 0;

/* Setting for 8 sub-buffers */
volatile uint32_t nway_ctrl[(1 << NUM_CHUNK)] = {0, 0, 0, 0, 0, 0, 0, 0};  

/* variables local to primary */
static uint8_t local_ctrl_idx = 0;
static uint32_t local_base = 0;

static bool
inline rbuf_overflow()
{
  return (*local_rbuf_idx - local_base) > NWAY_BOUND;
}

static void
inline reset_rbuf_idx(UINT32 ent)
{

  //adding control command
  rbuf[(*local_rbuf_idx)++] = RBUF_WRAP;

  // update ctrl_array -- access to shared variable
  nway_ctrl[local_ctrl_idx] = *local_rbuf_idx - local_base;

  //local update
  local_base =  *local_rbuf_idx;

  // move local index
  local_ctrl_idx = ((local_ctrl_idx + 1) & ((1 << NUM_CHUNK) - 1));

  // polling for progress -- access to shared variable
  while (nway_ctrl[local_ctrl_idx] != 0) {
    usleep(50);
  }

  //resetting local idx
  if (local_ctrl_idx == 0) {
    *local_rbuf_idx = 0;
    local_base = 0;
  }
  rbuf[(*local_rbuf_idx)++] = ent;
}

static void
inline reset_rbuf_idx_no_sec_chk(UINT32 ent)
{

  //adding control command
  rbuf[(*local_rbuf_idx)++] = RBUF_WRAP;

  // update ctrl_array -- access to shared variable
  nway_ctrl[local_ctrl_idx] = *local_rbuf_idx - local_base;

  //local update
  local_base =  *local_rbuf_idx;

  // move local index
  local_ctrl_idx = ((local_ctrl_idx + 1) & ((1 << NUM_CHUNK) - 1));

  // polling for progress -- access to shared variable
  while (nway_ctrl[local_ctrl_idx] != 0) {
    usleep(50);
  }

  //resetting local idx
  if (local_ctrl_idx == 0) {
    *local_rbuf_idx = 0;
    local_base = 0;
  }
  rbuf[(*local_rbuf_idx)++] = ent;
}


int main (int argc, char* argv[]){
    ;
}
