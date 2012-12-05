#include <stdio.h>
#include "fable.h"
#include "fable_helpers.h"

int main(int argc, char* argv[]) {
  fable_init();
  void* listen_handle = fable_listen("fooble_sock");
  printf("listen handle: %p\n", listen_handle);
  fable_set_nonblocking(listen_handle);
  void* new_handle;
  while (!(new_handle = fable_accept(listen_handle, FABLE_DIRECTION_RECEIVE))) {
  }
  CHECK_ERROR((errno != EAGAIN && errno != EINTR));
  printf("conn handle: %p\n", new_handle);
  char buf[1024];
  int i = 0;
  while (i < 10000000) {
    fable_read_all(new_handle, &buf[0], 1024);
    //printf("%d\n", *((int*)buf));
    if (i % 10000 == 0)
      printf("%d\n", i);
    ++i;
  }
  fable_close(new_handle);
  fable_close(listen_handle);
}
