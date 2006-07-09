#include<malloc.h>
#include<stdio.h>
#include<time.h>

#include<common/Logging.h>
#include<common/Hashtable.h>
#include<common/MemoryChunks.h>
#include<common/MemoryRomizer.h>

int total_req = 0;
int new_calls = 0;
int free_calls = 0;

extern "C" {
  int get_total_req(){ return total_req; }
  int get_new_calls(){ return new_calls; }
  int get_free_calls(){ return free_calls; }
}

bool romizer_collecting = false;
int total_size = 0, total_count = 0;

char *cloned = null;
int cloned_filled = 0;

void romizer_start()
{
  romizer_collecting = true;
}

void romizer_stop()
{
  romizer_collecting = false;
}


void romizer_traverse_i(int *address)
{
  int attr = address[-1];

  if (attr != 0xCAFE) {
    return;
  }
  
  int size = address[-2];
  assert(size >= 0 && size < 0x1000000);

  CLR_TRACE("MEM", "traverse %p - %d", address, size);
  
  // mark traversed
  address[-1] = 0xDEAD;

  // clone it
  if (cloned == null) {
    cloned = new char[total_size];
  }
  int this_filled = cloned_filled;
  memcpy(cloned+this_filled, address, size);
  cloned_filled += size;
  address[-3] = (int)(cloned+this_filled);

  // traverse in recurse
  for(int idx = 0; idx < size/4; idx++){
    int *new_address = ((int**)address)[idx];
    
    if ((int)new_address > 0x10000 && chunk_belongs(new_address)) {
      romizer_traverse_i(new_address);
      if (new_address[-1] == 0xDEAD) {
        ((int*) (cloned+this_filled) )[idx] = new_address[-3];
      }
    }
  }
}

void *romizer_traverse(void *address)
{
  bool local_rc = romizer_collecting;
  romizer_collecting = false;

  if (!chunk_belongs(address)) {
    CLR_WARN("MEM", "traverse: warn - unmapped address: %p", address);
    romizer_collecting = local_rc;
    return 0;
  }
  romizer_traverse_i((int*)address);
  
  CLR_TRACE("MEM", "traverse: cloned:%p, cloned_filled:%d", cloned, cloned_filled);

  romizer_collecting = local_rc;
  return (void*)((int*)address)[-3];
}




void *new_wrapper(size_t size)
{
#if MEMORY_PROFILE
  total_req += size;
  new_calls++;
#endif
  
  if (romizer_collecting) {
    romizer_collecting = false;
    int *ret = (int*)chunk_alloc(size + sizeof(int)*3);
    ret += 3;

    ret[-1] = 0xCAFE;
    ret[-2] = size;
    ret[-3] = 0x1;
    
    total_size += size;
    total_count++;
    CLR_TRACE("MEM:ROM", "%p[%d] - %d/%d  [%d K]", ret, size, 0, total_count, total_size/1024);
    
    romizer_collecting = true;
    return ret;
  } else {
    return malloc(size);
  }
}

void delete_wrapper(void *ptr)
{
#if MEMORY_PROFILE
  free_calls++;
#endif

  if (romizer_collecting) {
    romizer_collecting = false;
    int *ret = (int*)ptr;
    
    if (chunk_belongs(ret)) 
    {
      ret[-1] = 0xBABE;
      ret[-3] = 0x1;
      total_size -= ret[-2];
      total_count--;
      CLR_TRACE("MEM:REM", "%p[%d] - %d  [%d K]", ret, ret[-2], total_count, total_size/1024);
      ret -= 3;
    }
    chunk_free(ret);
    romizer_collecting = true;
  } else if (ptr > cloned && ptr <= cloned + cloned_filled) {
    return;
  } else {
    free(ptr);
  }
  return;
}
