#include <stdio.h>
#include "common.h"
#include "fable.h"
#include "fable_helpers.h"

int main(int argc, char* argv[]) {
  fable_init();
  void* new_handle = fable_connect("fooble_sock", FABLE_DIRECTION_SEND);
  CHECK_ERROR(!new_handle);
  printf("conn handle: %p\n", new_handle);
  //int buf = 42;
  char buf[BUF_SIZE];
  while (true) {
    fable_write_all(new_handle, (char*)&buf, BUF_SIZE);
  }
  fable_close(new_handle);
}
