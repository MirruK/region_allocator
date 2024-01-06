#include "../include/region_allocator.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  region_allocator *region = init_region_allocator(0, (void *)8192);
  if (region == NULL) {
    perror("Failed to init region_allocator struct");
    return EXIT_FAILURE;
  }
  printf("Allocating some memory and printing the state of the regions\n");
  void *first = region_alloc(region, 1000, 0);
  print_regions(region);
  void *four_hundred_bytes = region_alloc(region, 400, 0);
  region_alloc(region, 2000, 1);
  print_regions(region);
  region_free(region, (void *)four_hundred_bytes);
  four_hundred_bytes = NULL;
  print_regions(region);
  region_free(region, (void *)first);
  print_regions(region);
  // Free allocated memory chunk and related structs
  destroy_region_allocator(region);
  // Invalidate pointer
  region = NULL;

  return EXIT_SUCCESS;
}
