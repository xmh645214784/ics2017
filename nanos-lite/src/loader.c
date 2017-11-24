#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

void init_ramdisk();
size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  int fd = fs_open(filename, 0, 0);
  if (fd == -1) {
    panic("file '%s' not found.", filename);
  }
  fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  return (uintptr_t)DEFAULT_ENTRY;
}
