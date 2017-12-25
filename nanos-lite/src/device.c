#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

extern int current_game;
extern uint32_t* const fb;
size_t events_read(void *buf, size_t len) {
  short key_event = (short)_read_key();
  if (key_event < 0) {
    key_event ^= 0x8000;
    snprintf(buf, len, "kd %s\n", keyname[key_event]);
    if (key_event == 3)
      current_game = !current_game;
  }
  else if (key_event > 0)
    snprintf(buf, len, "ku %s\n", keyname[key_event]);
  else
    snprintf(buf, len, "t %d\n", _uptime());
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void ramdisk_read(void *buf, off_t offset, size_t len);
void dispinfo_read(void *buf, off_t offset, size_t len) {
    memcpy(buf, (char*)dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  memcpy((char *)fb + offset, buf, len);
}

void ramdisk_write(const void *buf, off_t offset, size_t len);
void init_device() {
  _ioe_init();
  sprintf(dispinfo, "WIDTH : %d\nHEIGHT:%d\n", _screen.width, _screen.height);

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
