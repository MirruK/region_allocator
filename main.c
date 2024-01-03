#include "mem_region.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  mem_regions *region = init_mem_regions(0, (void *)8192);
  if (region == NULL) {
    perror("Failed to init mem_regions struct");
    return EXIT_FAILURE;
  }
  /*int *thousand_bytes = (int *)*/ region_alloc(region, 1000, 0);
  print_regions(region);
  void *four_hundred_bytes = region_alloc(region, 400, 0);
  /*int *two_thousand_bytes = (int *)*/ region_alloc(region, 2000, 1);
  print_regions(region);
  region_free(region, (void *)four_hundred_bytes);
  four_hundred_bytes = NULL;
  print_regions(region);
  // Free allocated memory chunk and related structs
  destroy_mem_regions(region);
  // Invalidate pointer
  region = NULL;

  return EXIT_SUCCESS;
}
