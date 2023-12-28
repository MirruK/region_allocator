#include "mem_region.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  region_range *first_region = malloc(sizeof(region_range));
  memcpy(first_region, root_region, sizeof(region_range));
  // root_region should never be modified and always come first
  // A copy of it is made and set to be the actual first allocatable region of
  // memory, first_region starts off by having no "next" region
  root_region->next = first_region;
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
  while (curr != NULL) {
    temp = curr;
    curr = curr->next;
    free(temp);
  }
  // Free the actual memory region
  free(regions->start_addr);
  free(regions);
}

void print_regions(mem_regions *mem_regions) {
  size_t chunk_size = mem_regions->end_addr - mem_regions->start_addr;
  int print_width = 80;
  int bytes_per_char = chunk_size / print_width;
  region_range *curr = mem_regions->root;
  while ((curr = curr->next) != NULL) {
    size_t curr_chunk_size = curr->end_addr - curr->start_addr;
    char char_to_print = 'X';
    if (curr->region_type == UNRESERVED) {
      char_to_print = '_';
    }
    char print_buff[80];
    int i = 0;
    for (i = 0; i < (curr_chunk_size / bytes_per_char); i++) {
      print_buff[i] = char_to_print;
    }
    print_buff[i] = '|';
    print_buff[i + 1] = '\0';
    printf("%s", print_buff);
  }
  printf("\n");
}

region_range *first_match(mem_regions *mem_regions, region_range *curr,
                          size_t nbytes) {
  static int retries = 0;
  if (curr->region_type == UNRESERVED &&
      (curr->end_addr - curr->start_addr) >= nbytes) {
    return curr;
  }
  // No unreserved region was adequately sized
  // Defragment memory and retry once
  if (curr->next == NULL) {
    if (retries == 0) {
      // Defragment
      //  defragment_regions(mem_region);
      retries++;
      first_match(mem_regions, mem_regions->root->next, nbytes);
    }
    // Defragmenting did not help, fail
    retries = 0;
    return NULL;
  }
  return first_match(mem_regions, curr->next, nbytes);
}

void split_regions(region_range *reserved_region, size_t nbytes) {
  region_range *unreserved_region = malloc(sizeof(region_range));
  memcpy(unreserved_region, reserved_region, sizeof(region_range));
  // New range represents the new unreserved smaller portion of memory
  reserved_region->end_addr = reserved_region->start_addr + nbytes;
  unreserved_region->start_addr += nbytes + 1;
  reserved_region->region_type = RESERVED;
  reserved_region->next = unreserved_region;
}

void *region_alloc(mem_regions *regions, size_t nbytes) {
  region_range *matched_region = NULL;
  /* First match allocation */
  if ((matched_region = first_match(regions, regions->root->next, nbytes)) !=
      NULL) {
    // Do not split if the allocated region is exactly the size of matched
    // region
    if ((matched_region->end_addr - matched_region->start_addr) > nbytes) {
      split_regions(matched_region, nbytes);
      // Increment length of regions linked list
      regions->len++;
    }
    matched_region->region_type = RESERVED;
    return matched_region->start_addr;
  }
  return NULL;
}