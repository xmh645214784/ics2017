#include "fs.h"

void ramdisk_read(void *buf, off_t offset, size_t len);
void ramdisk_write(const void *buf, off_t offset, size_t len);
void fb_write(const void *buf, off_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[3].size = _screen.width * _screen.height * sizeof(uint32_t);
}

int fs_open(const char *pathname, int flags, int mode) {
  int i = 0;
  for (; i < NR_FILES; ++i) {
    if (strcmp(file_table[i].name, pathname) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(0);
  return -1;
}

size_t events_read(void *buf, size_t len);
void dispinfo_read(void *, off_t, size_t);
ssize_t fs_read(int fd, void *buf, size_t len) {
  if (fd == FD_EVENTS) return events_read(buf, len);
  if (len + file_table[fd].open_offset > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  if (fd != FD_DISPINFO)
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  else
    dispinfo_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  if (len + file_table[fd].open_offset > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  if (fd == FD_FB) {
    fb_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  }
  else {
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += len;
  return len;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  switch (whence) {
    case SEEK_SET: file_table[fd].open_offset = offset; break;
    case SEEK_CUR: file_table[fd].open_offset += offset; break;
    case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
    default: return -1;
  }
  if (file_table[fd].open_offset > file_table[fd].size)
    file_table[fd].open_offset = file_table[fd].size;
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  return 0;
}

size_t fs_filesz(int fd) {
  return file_table[fd].size;
}