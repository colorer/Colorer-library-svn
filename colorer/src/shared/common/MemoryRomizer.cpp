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
SortedVector<int*> *romizer_data = null;
int **cloned_data = null;
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


void romizer_traverse_i(int addr_idx)
{
  int *address = romizer_data->elementAt(addr_idx);
  
  int attr = address[-1];
  assert(attr != 0xCAFE || attr != 0xDEAD);
  if (attr == 0xDEAD) {
    return;
  }
  
  int size = ((int*)address)[-2];
  clr_assert(size >= 0 && size < 0x1000000);

  CLR_TRACE("MEM", "traverse %p - %d", address, size);
  
  // mark traversed
  address[-1] = 0xDEAD;

  // clone it
  if (cloned == null) {
    cloned = new char[total_size];
    cloned_data = new int*[romizer_data->size()];
  }
  int this_filled = cloned_filled;
  memcpy(cloned+this_filled, address, size);
  cloned_filled += size;
  cloned_data[addr_idx] = (int*)(cloned+this_filled);

  // traverse in recurse
  for(int idx = 0; idx < size/4; idx++){
    int *new_address = ((int**)address)[idx];
    int new_idx = romizer_data->indexOf(new_address);
    if ((int)new_address > 0x10000 && new_idx != -1) {
      romizer_traverse_i(new_idx);
      ((int**) (cloned+this_filled) )[idx] = cloned_data[new_idx];
    }
  }
}

void *romizer_traverse(void *address)
{
  bool local_rc = romizer_collecting;
  romizer_collecting = false;

  int addr_idx = romizer_data->indexOf((int*)address);
  if (addr_idx == -1) {
    CLR_WARN("MEM", "traverse: warn - unmapped address: %p", address);
    romizer_collecting = local_rc;
    return 0;
  }
  romizer_traverse_i(addr_idx);
  
  CLR_TRACE("MEM", "traverse: cloned:%p, cloned_filled:%d", cloned, cloned_filled);

  romizer_collecting = local_rc;
  return cloned_data[addr_idx];
}




void *new_wrapper(size_t size)
{
#if MEMORY_PROFILE
  total_req += size;
  new_calls++;
#endif
  
  if (romizer_collecting) {
    romizer_collecting = false;
    int *ret = (int*)malloc(size + sizeof(int)*2);
    ret[0] = size;
    ret[1] = 0xCAFE;
    if (romizer_data == null) {
      romizer_data = new SortedVector<int*>(100000);
    }
    ret += 2;
    
    total_size += size;
    total_count++;
    CLR_TRACE("MEM:ROM", "%p[%d] - %d/%d  [%d K]", ret, size, romizer_data->size(), total_count, total_size/1024);
    
    romizer_data->addElement(ret);
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
    
    int idx = romizer_data->indexOf(ret);
    if (idx != -1) 
    {
      ret[-1] = 0xBABE;
      total_size -= ret[-2];
      CLR_TRACE("MEM:REM", "%p[%d] - %d  [%d K]", ret, ret[-2], romizer_data->size(), total_size/1024);
      romizer_data->removeElementAt(idx);
      ret -= 2;
    }
    free(ret);
    romizer_collecting = true;
  } else if (ptr > cloned && ptr <= cloned + cloned_filled) {
    return;
  } else {
    free(ptr);
  }
  return;
}
