#include "../include/region_allocator.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

region_allocator *init_region_allocator(void *start_addr, void *end_addr) {
  uintptr_t region_size = (uintptr_t)end_addr - (uintptr_t)start_addr;
  // Malloc the actual memory block that will be used
  uintptr_t region = (uintptr_t)malloc(region_size);
  region_allocator *regions_ls = malloc(sizeof(region_allocator));
  memory_region *root_region = malloc(sizeof(memory_region));
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
  memory_region *first_region = malloc(sizeof(memory_region));
  if (first_region == NULL) {
    perror("malloc error: ");
    return NULL;
  }
  memcpy(first_region, root_region, sizeof(memory_region));
  // root_region should never be modified and always come first
  // A copy of it is made and set to be the actual first allocatable region of
  // memory, first_region starts off by having no "next" region
  root_region->next = first_region;
  regions_ls->start_addr = actual_start_addr;
  regions_ls->end_addr = actual_end_addr;
  regions_ls->len = 1;
  regions_ls->root = root_region;
  regions_ls->root->prev = NULL;
  regions_ls->head = root_region->next;
  regions_ls->head->prev = NULL;
  return regions_ls;
}

void destroy_region_allocator(region_allocator *allocator) {
  if (allocator == NULL) {
    // No work to be done here
    return;
  }
  memory_region *curr = allocator->root;
  memory_region *temp = curr;
  // Free all memory_region sturcts in linked list
  while (curr != NULL) {
    temp = curr;
    curr = curr->next;
    free(temp);
  }
  // Free the actual memory region
  free(allocator->start_addr);
  free(allocator);
}

void print_regions(region_allocator *allocator) {
  // Total size of memory
  size_t chunk_size =
      (char *)allocator->end_addr - (char *)allocator->start_addr;
  int print_width = 80;
  // How many bytes of actual memory will be represented by a single character
  int bytes_per_char = chunk_size / print_width;
  memory_region *curr = allocator->root;
  char print_buff[print_width + 1];
  while ((curr = curr->next) != NULL) {
    size_t curr_chunk_size = (char *)curr->end_addr - (char *)curr->start_addr;
    char char_to_print = 'X';
    if (curr->region_type == UNRESERVED) {
      char_to_print = '_';
    }
    size_t i = 0;
    for (i = 0; i < (curr_chunk_size / bytes_per_char); i++) {
      print_buff[i] = char_to_print;
    }
    print_buff[i] = '|';
    print_buff[i + 1] = '\0';
    printf("%s", print_buff);
  }
  printf("\n");
}

static memory_region *next_match(region_allocator *allocator,
                                 memory_region *curr, size_t nbytes) {
  static int retries = 0;
  if (curr->region_type == UNRESERVED &&
      ((char *)curr->end_addr - (char *)curr->start_addr) >= (long)nbytes) {
    return curr;
  }
  // No unreserved region was adequately sized
  // Defragment memory and retry once
  if (curr->next == NULL) {
    if (retries == 0) {
      // Defragment
      /* TODO: Uncomment this and test after implementing defragment function
      defragment_region_allocator(region_allocator); */
      retries++;
      next_match(allocator, allocator->root->next, nbytes);
    }
    // Defragmenting did not help, fail
    retries = 0;
    return NULL;
  }
  return next_match(allocator, curr->next, nbytes);
}

static void split_regions(memory_region *reserved_region, size_t nbytes) {
  memory_region *unreserved_region = malloc(sizeof(memory_region));
  if (unreserved_region == NULL) {
    perror("malloc error: ");
    return;
  }
  memcpy(unreserved_region, reserved_region, sizeof(memory_region));
  // New range represents the new unreserved smaller portion of memory
  reserved_region->end_addr =
      (void *)((char *)reserved_region->start_addr + nbytes);
  unreserved_region->start_addr =
      (void *)((char *)unreserved_region->start_addr + nbytes + 1);
  if (reserved_region->next != NULL) {
    reserved_region->next->prev = unreserved_region;
  }
  reserved_region->region_type = RESERVED;
  reserved_region->next = unreserved_region;
  unreserved_region->prev = reserved_region;
}

/* Helper function to merge region1 with a region on region1's right side. */
static void merge_regions(memory_region *region1, memory_region *region2) {
  // Panic if function is called inappropriately (i.e. regions aren't both
  // unreserved)
  assert(region1->region_type == UNRESERVED);
  assert(region2->region_type == UNRESERVED);

  // [p]->[1]->[2]->[n] --> [p]->[1-2]->[n]
  region1->next = region2->next;
  region1->end_addr = region2->end_addr;
}

void *region_alloc(region_allocator *allocator, size_t nbytes, int match_type) {
  memory_region *matched_region = NULL;
  switch (match_type) {
  case 0:
    matched_region = next_match(allocator, allocator->root->next, nbytes);
    break;
  default:
    matched_region = next_match(allocator, allocator->head, nbytes);
    break;
  }
  /* First match allocation */
  if (matched_region != NULL) {
    // Do not split if the allocated region is exactly the size of matched
    // region
    if (((char *)matched_region->end_addr -
         (char *)matched_region->start_addr) > (long)nbytes) {
      split_regions(matched_region, nbytes);
      // Increment length of regions linked list
      allocator->len++;
      allocator->head = matched_region->next;
    }
    matched_region->region_type = RESERVED;
    return matched_region->start_addr;
  }
  return NULL;
}

static void defragment_region_allocator(void) __attribute__((unused));
static void defragment_region_allocator(void) { /* TODO: IMPLEMENT HOW? */
}

static memory_region *find_region(region_allocator *allocator,
                                  void *addr_to_find) {
  memory_region *curr = allocator->root->next;
  while (curr->next != NULL) {
    if (curr->start_addr == addr_to_find) {
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}

void region_free(region_allocator *allocator, void *addr_to_free) {
  memory_region *region_to_free = find_region(allocator, addr_to_free);
  if (region_to_free == NULL) {
    printf("No region matching address provided was found, no memory will be "
           "freed.\nFree that address at your own risk.");
    return;
  }
  region_to_free->region_type = UNRESERVED;
  if (region_to_free->next != NULL &&
      region_to_free->next->region_type == UNRESERVED) {
    merge_regions(region_to_free, region_to_free->next);
  }
  if (region_to_free->prev != NULL &&
      region_to_free->prev->region_type == UNRESERVED) {
    merge_regions(region_to_free->prev, region_to_free);
  }
}
