#include "mem_region.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  mem_regions *region = init_mem_regions(0, (void *)8192);
  if (region == NULL) {
    perror("Failed to init mem_regions struct");
    return EXIT_FAILURE;
  }
  print_regions(region);
  printf("Mem region start and end addr: %p, %p\n", region->start_addr,
         region->end_addr);
  printf("Mem region root start and end addr: %p, %p\n",
         region->root->start_addr, region->root->end_addr);
  int *thousand_bytes = (int *)region_alloc(region, 1000);
  thousand_bytes[10] = 5;
  printf(
      "Allocated 1000 bytes starting at %p, set a value at offset 10 to %d\n",
      thousand_bytes, thousand_bytes[10]);
  int *two_thousand_bytes = (int *)region_alloc(region, 200);
  two_thousand_bytes[10] = 2148;
  printf(
      "Allocated 2000 bytes starting at %p, set a value at offset 10 to %d\n",
      two_thousand_bytes, two_thousand_bytes[10]);
  print_regions(region);
  // Free allocated memory chunk and related structs
  destroy_mem_regions(region);
  // Invalidate pointer
  region = NULL;

  return EXIT_SUCCESS;
}