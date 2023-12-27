#include "linkedlist.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  mem_regions *region = init_mem_regions(0, (void *)0xFFF);
  if (region == NULL) {
    perror("Failed to init mem_regions struct");
    return EXIT_FAILURE;
  }
  printf("Mem region start and end addr: %p, %p\n", region->start_addr,
         region->end_addr);
  printf("Mem region root start and end addr: %p, %p\n",
         region->root->start_addr, region->root->end_addr);
  destroy_mem_regions(region);
  return EXIT_SUCCESS;
}