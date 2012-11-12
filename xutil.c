/*
    Copyright (c) 2011 Anil Madhavapeddy <anil@recoil.org>
    Copyright (c) 2011 Steven Smith <sos22@cam.ac.uk>
    Copyright (c) 2012 Malte Schwarzkopf <ms705@cam.ac.uk>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/
#include <sys/mman.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <numa.h>
#include <sched.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <inttypes.h>
#include "atomicio.h"
#include "xutil.h"

void *
xmalloc(size_t size)
{
  void *buf;
  buf = malloc(size);
  if (buf == NULL)
    err(1, "xmalloc");
  return buf;
}

void
xread(int fd, void *buf, size_t count)
{
  ssize_t r;
  r = atomicio(read, fd, buf, count);
  if (r != count)
    err(1, "xread");
}

void
xwrite(int fd, const void *buf, size_t count)
{
  ssize_t r;
  r = atomicio(vwrite, fd, (void *)buf, count);
  if (r != count)
    err(1, "xwrite");
}

void
setaffinity(int cpunum)
{
  cpu_set_t *mask;
  size_t size;
  int i;
  int nrcpus = 160;
  pid_t pid;
  mask = CPU_ALLOC(nrcpus);
  size = CPU_ALLOC_SIZE(nrcpus);
  CPU_ZERO_S(size, mask);
  CPU_SET_S(cpunum, size, mask);
  pid = getpid();
  i = sched_setaffinity(pid, size, mask);
  if (i == -1)
    err(1, "sched_setaffinity");
  CPU_FREE(mask);
}

void *
establish_shm_segment(int nr_pages, int numa_node)
{
  int fd;
  void *addr;
  struct bitmask *alloc_nodes;
  struct bitmask *old_mask;

  fd = shm_open("/memflag_lat", O_RDWR|O_CREAT|O_EXCL, 0600);
  if (fd < 0)
    err(1, "shm_open(\"/memflag_lat\")");
  shm_unlink("/memflag_lat");
  if (ftruncate(fd, PAGE_SIZE * nr_pages) < 0)
    err(1, "ftruncate() shared memory segment");
  addr = mmap(NULL, PAGE_SIZE * nr_pages, PROT_READ|PROT_WRITE, MAP_SHARED,
	      fd, 0);
  if (addr == MAP_FAILED)
    err(1, "mapping shared memory segment");

  if(numa_node != -1)
    numa_tonode_memory(addr, PAGE_SIZE * nr_pages, numa_node);

  close(fd);

  return addr;
}
