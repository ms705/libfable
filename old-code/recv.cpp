#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "fable.h"
#include "fable_helpers.h"

#define __USE_BSD  // for timesub

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
  char buf[BUF_SIZE];
  int i = 0;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  printf("timer started\n");
  while (i < 10000000) {
    fable_read_all(new_handle, &buf[0], BUF_SIZE);
    //printf("%d\n", *((int*)buf));
    //if (i % 10000 == 0)
    //  printf("%d\n", i);
    ++i;
  }
  gettimeofday(&end, NULL);
  struct timeval diff;
  timersub(&end, &start, &diff);
  printf("time elapsed: %lds, %ldµs\n", diff.tv_sec, diff.tv_usec);
  double bw = (double)(BUF_SIZE*10000000L) / (double)(diff.tv_sec);
  printf("bandwidth: %f bytes/sec\n", bw);
  printf("= %f Gbps\n", ((bw / (1024.0*1024.0*1024.0)) * 8.0));
  fable_close(new_handle);
  fable_close(listen_handle);
}
