#include "xiod.h"

/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#define MAX_LINE 16384

void do_read(evutil_socket_t fd, short events, void *arg);
void do_write(evutil_socket_t fd, short events, void *arg);

void
handle_request(char* line, struct evbuffer *output) {
  char* fn;
  printf("Received: %s\n", line);
  fn = strtok(line, " ");
  if (strcmp(fn, "REG") == 0) {
    // Registration
    char* endpt = strtok(NULL, " ");
    int cap;
    printf("Registering endpoint %s \n", endpt);
    // Create endpoint record
    xio_nd_rec* rec = (xio_nd_rec*)malloc(sizeof(xio_nd_rec));
    rec->name = strcpy((char*)malloc(strlen(endpt)), endpt);
    // Add capabilities presented to endpoint record
    if (!(cap = atoi(strtok(NULL, " ")))) {
      printf("ERROR: invalid transports specified\n");
      return;
    }
    rec->transports = cap;
    printf("Capabilities: %d\n", rec->transports);
    evbuffer_add(output, "DONE\n", 5);
  } else if (strcmp(fn, "RES") == 0) {
    // Name resolution
    char* endpt = strtok(NULL, " ");
    printf("Resolving endpoint %s\n", endpt);
    //evbuffer_add(output, "DONE\n", 5);
  } else {
    printf("ERROR: unknown command, must be one of REG/RES\n");
  }
}

void
readcb(struct bufferevent *bev, void *ctx)
{
  struct evbuffer *input, *output;
  char *line;
  size_t n;
  input = bufferevent_get_input(bev);
  output = bufferevent_get_output(bev);

  while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
    // hack to remove line feed for processing purposes
    line[strlen(line)-1] = '\0';
    handle_request(line, output);
    free(line);
  }

  if (evbuffer_get_length(input) >= MAX_LINE) {
    // Input too long; just return error
    printf("ERROR: input too long\n");
    evbuffer_add(output, "ERROR\n", 6);
  }
}

void
errorcb(struct bufferevent *bev, short error, void *ctx)
{
  if (error & BEV_EVENT_EOF) {
    /* connection has been closed, do any clean up here */
    /* ... */
  } else if (error & BEV_EVENT_ERROR) {
    /* check errno to see what error occurred */
    /* ... */
  } else if (error & BEV_EVENT_TIMEOUT) {
    /* must be a timeout event handle, handle it */
    /* ... */
  }
  bufferevent_free(bev);
}

void
do_accept(evutil_socket_t listener, short event, void *arg)
{
  struct event_base *base = arg;
  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int fd = accept(listener, (struct sockaddr*)&ss, &slen);
  if (fd < 0) {
    perror("accept");
  } else if (fd > FD_SETSIZE) {
    close(fd);
  } else {
    struct bufferevent *bev;
    evutil_make_socket_nonblocking(fd);
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
    bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
  }
}

void
run(void)
{
  evutil_socket_t listener;
  struct sockaddr_in sin;
  struct event_base *base;
  struct event *listener_event;

  base = event_base_new();
  if (!base)
    return; /*XXXerr*/

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(40713);

  listener = socket(AF_INET, SOCK_STREAM, 0);
  evutil_make_socket_nonblocking(listener);

  {
    int one = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  }

  if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
    perror("bind");
    return;
  }

  if (listen(listener, 16)<0) {
    perror("listen");
    return;
  }

  listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
  /*XXX check it */
  event_add(listener_event, NULL);

  event_base_dispatch(base);
}

int
main(int c, char **v)
{
  setvbuf(stdout, NULL, _IONBF, 0);

  run();
  return 0;
}
