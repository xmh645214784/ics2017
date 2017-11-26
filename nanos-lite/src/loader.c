#include "common.h"
#include "fs.h"
#include "memory.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

void init_ramdisk();
size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  int fd = fs_open(filename, 0, 0);
  if (fd == -1) {
    panic("file '%s' not found.", filename);
  }
  uint32_t pages = ((fs_filesz(fd) - 1) >> 12) + 1;
  void *vaddr = DEFAULT_ENTRY;
  for (; pages--; vaddr += PGSIZE) {
    void *paddr = new_page();
    _map(as, vaddr, paddr);
    fs_read(fd, paddr, PGSIZE);
  }
  return (uintptr_t)DEFAULT_ENTRY;
}
