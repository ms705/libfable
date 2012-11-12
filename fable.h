// Interface definition strub for libfable, as per RESoLVE paper.

#ifndef LIBFABLE_INTERFACE_H_
#define LIBFABLE_INTERFACE_H_

#include <stdint.h>
#include <stdio.h>

// ---------------------------------------------
// Data structures
// ---------------------------------------------

typedef struct {
  // Context ID
  uint64_t c_id;
  // File descriptor for communication with kernel nameservice
  FILE *fd;
} xio_context_t;

typedef struct {
  // Contect this connection belongs to.
  xio_context_t *ctx;

  // FD for polling on reconfiguration events
  FILE *fd;
} xio_handle_t;

typedef struct {
  // Actual I/O memory described by this handle
  void *buf_base;
  size_t buf_len;

  // Epoch number for reconfiguration purposes
  uint64_t epoch;

  // "Home" connection of this buffer.
  xio_handle_t* handle;
} xio_buffer_t;

typedef struct {
  size_t buf_size;
} xio_config_t;

// ---------------------------------------------
// Methods
// ---------------------------------------------

// Registers with the userspace library and the kernel name server using the
// name provided in the argument.
xio_context_t* xio_register_name(char *name);

// Connects to a remote endpoint using the specified xio_context. Returns an
// xio_handle representing the connection.
xio_handle_t* xio_connect(xio_context_t *ctx, char *remote_uri);

// Listens for connection attempts from a remote end point using the specified
// xio_context. Blocking function call. The xio_handle returned represents the
// connection attempt we received.
xio_handle_t* xio_listen(xio_context_t *ctx);

// Accepts a connection attempt represented by the specified xio_handle.
void xio_accept(xio_handle_t *handle);

// Obtains the next set of read buffers.
// ARGUMENTS:
// 1) a pointer to the xio_handle representing this connection.
// 2) a pointer to a pointer; this will be set to a pointer to the xio_buffer
//    array holding pointers to the next set of read buffers.
// RETURN VALUE:
// - The number of read buffers in the array at the pointer supplied back in
//   the first argument (which may be zero), or ENOSPACE if the limit on the
//   number of buffers has been exceeded.
uint64_t xio_getreadbuf(xio_handle_t* handle, xio_buffer_t** buf_ptrs);

// Obtains a set of write buffers.
// ARGUMENTS:
// 1) a pointer to the xio_handle representing this connection.
// 2) a pointer to a pointer; this will be set to a pointer to the xio_buffer
//    array holding pointers to the next set of read buffers.
// 3) an optional size hint; set to 0 to let the library choose a suitable
//    buffer size. Note that even if this argument is supplied, the library may
//    choose to supply buffers of a different size.
// RETURN VALUE:
// - The number of write buffers in the array at the pointer supplied back in
//   the first argument (must be > 0), or ENOSPACE if the limit on the number
//   of buffers has been exceeded.
uint64_t xio_getwritebuf(xio_handle_t* handle, xio_buffer_t** buf_ptrs,
                         size_t size);

// Instructs the library to transfer ownership of the buffer pointed to by the
// argument to the receiving end of the connection.
void xio_commmit(xio_buffer_t *buf);

// Relinquishes the buffer pointed to by the argument for reuse by the library.
void xio_release(xio_buffer_t *buf);

#endif  // FIRMAMENT_MISC_FABLE_INTERFACE_H_
