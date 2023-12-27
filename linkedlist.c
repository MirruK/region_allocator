#include "linkedlist.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

mem_regions *init_mem_regions(void *start_addr, void *end_addr) {
  uintptr_t region_size = (uintptr_t)end_addr - (uintptr_t)start_addr;
  // Ensure the region of addresses will actually be usable by the process
  uintptr_t region = (uintptr_t)malloc(region_size);
  mem_regions *regions_ls = malloc(sizeof(mem_regions));
  region_range *root_region = malloc(sizeof(region_range));
  if ((void *)region == NULL) {
    perror("Error when allocating region of memory");
    return NULL;
  }
  if (regions_ls == NULL) {
    perror("Error when allocating memory for region list struct");
    return NULL;
  }
  if (root_region == NULL) {
    perror("Error when allocating memory for region range struct");
    return NULL;
  }
  void *actual_start_addr = (void *)region;
  void *actual_end_addr = (void *)(region + region_size);
  root_region->region_type = UNRESERVED;
  root_region->next = NULL;
  root_region->start_addr = actual_start_addr;
  root_region->end_addr = actual_end_addr;
  regions_ls->start_addr = actual_start_addr;
  regions_ls->end_addr = actual_end_addr;
  regions_ls->len = 1;
  regions_ls->root = root_region;
  return regions_ls;
}

void destroy_mem_regions(mem_regions *regions) {
  if (regions == NULL) {
    // No work to be done here
    return;
  }
  region_range *curr = regions->root;
  region_range *temp = curr;
  // Free all region_range sturcts in linked list
  for (size_t i = 0; i < regions->len; i++) {
    if (curr == NULL) {
      break;
    }
    temp = curr;
    curr = curr->next;
    free(temp);
  }
  // Free the actual memory region
  free(regions->start_addr);
  free(regions);
  regions = NULL;
}