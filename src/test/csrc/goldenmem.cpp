#include <goldenmem.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

uint8_t *pmem;

void* guest_to_host(paddr_t addr) { return &pmem[addr]; }

void init_goldenmem() {
  pmem = (uint8_t *)mmap(NULL, PMEM_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (pmem == (uint8_t *)MAP_FAILED) {
    printf("ERROR allocating physical memory. \n");
  }
}

void update_goldenmem(paddr_t addr, void *data, uint64_t mask, int len) {
  uint8_t *dataArray = (uint8_t*)data;
  for (int i = 0; i < len; i++) {
		if (((mask >> i) & 1) != 0) {
      // if (addr+i == 0x8055fa5c) {
      //   printf("+++++++++++ 0x%x\n", dataArray[i]);
      // }
			paddr_write(addr + i, dataArray[i], 1);
		}
  }
}

void read_goldenmem(paddr_t addr, void *data, uint64_t len) {
  *(uint64_t*)data = paddr_read(addr, len);
}

static inline bool in_pmem(paddr_t addr) {
  return (PMEM_BASE <= addr) && (addr <= PMEM_BASE + PMEM_SIZE - 1);
}

static inline word_t pmem_read(paddr_t addr, int len) {
  void *p = &pmem[addr - PMEM_BASE];
  switch (len) {
    case 1: return *(uint8_t  *)p;
    case 2: return *(uint16_t *)p;
    case 4: return *(uint32_t *)p;
    case 8: return *(uint64_t *)p;
    default: assert(0);
  }
}

static inline void pmem_write(paddr_t addr, word_t data, int len) {
#ifdef DIFFTEST_STORE_COMMIT
  store_commit_queue_push(addr, data, len);
#endif

  // write to pmem, mark pmem addr as dirty
  void *p = &pmem[addr - PMEM_BASE];
  switch (len) {
    case 1: 
      *(uint8_t  *)p = data;
      return;
    case 2: 
      *(uint16_t *)p = data;
      return;
    case 4: 
      *(uint32_t *)p = data;
      return;
    case 8: 
      *(uint64_t *)p = data;
      return;
    default: assert(0);
  }
}

/* Memory accessing interfaces */

inline word_t paddr_read(paddr_t addr, int len) {
  if (in_pmem(addr)) return pmem_read(addr, len);
  else panic("read not in pmem! addr: %lx", addr);
  return 0;
}

inline void paddr_write(paddr_t addr, word_t data, int len) {
  if (in_pmem(addr)) pmem_write(addr, data, len);
  else panic("write not in pmem!");
}