#pragma once
#include <stddef.h>

/* Used to distinguish whether a memory_region struct represents allocated or
 * unallocated memory */
typedef enum RegionType { UNRESERVED = 0, RESERVED = 1 } RegionType;

/* Represents a segment of continuous memory that is either
    allocated or unallocated */
typedef struct memory_region {
  void *start_addr;
  void *end_addr;
  RegionType region_type;
  struct memory_region *next;
  struct memory_region *prev;
} memory_region;

/* A linked-list data structure to hold information about what parts of a chunk
   of memory are allocated or unallocated.
  */
typedef struct {
  void *start_addr;
  void *end_addr;
  size_t len;
  memory_region *root;
  memory_region *head;
} region_allocator;

/* Initialize a region_allocator data structure */
region_allocator *init_region_allocator(void *start_addr, void *end_addr);

/* Free region_allocator and all associated structs */
void destroy_region_allocator(region_allocator *allocator);

/* Print the contents of the region_allocator (mainly for debugging purposes) */
void print_regions(region_allocator *allocator);

/* Allocate memory from a region_allocator for use.
  Returns: A pointer to the start of the allocated memory
  or NULL if an error occurred.
  The "match-type" given determines what algorithm is used.
  0: first match.
  default: next match*/
void *region_alloc(region_allocator *allocator, size_t nbytes, int match_type);

/* Mark "range_to_free" as unreserved.
  If region is now adjacent to other unreserved regions, merge them */
void region_free(region_allocator *allocator, void *addr_to_free);
