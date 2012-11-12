#include <stdio.h>

#include "fable.h"

int run_test() {
  // Register local endpoint
  xio_context_t* ctx = xio_register_name("testendpt");
  // Connect to remote endpoint
  xio_connect(ctx, "testremoteendpt");
  return 0;
}

int main() {
  if (!run_test())
    printf("FAILED\n");
  else
    printf("SUCCEEDED\n");
}
