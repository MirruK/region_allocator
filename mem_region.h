#pragma once
#include <stddef.h>

/* Represents an ordering (Less than, Equal or Greater than)*/
typedef enum Ord { LT = -1, EQ = 0, GT = 1 } Ord;
/* Used to distinguish whether a region_range struct represents allocated or
 * unallocated memory */
typedef enum RegionType { UNRESERVED = 0, RESERVED = 1 } RegionType;

/* Represents a segment of continuous memory that is either
    allocated or unallocated */
typedef struct region_range {
  void *start_addr;
  void *end_addr;
  RegionType region_type;
  struct region_range *next;
  struct region_range *prev;
} region_range;

/* A linked-list data structure to hold information about what parts of a chunk
   of memory are allocated or unallocated*/
typedef struct mem_regions {
  void *start_addr;
  void *end_addr;
  size_t len;
  region_range *root;
  region_range *head;
} mem_regions;

/* Initialize a mem_regions data structure */
mem_regions *init_mem_regions(void *start_addr, void *end_addr);

/* Free mem_region and all associated structs */
void destroy_mem_regions(mem_regions *mem_regions);

/* Print the contents of a mem_regions list (mainly for debugging purposes) */
void print_regions(mem_regions *mem_regions);

/* Allocate memory from a mem_region for use.
  Returns: A pointer to the start of the allocated memory
  or NULL if an error occurred.
  The "match-type" given determines what algorithm is used.
  0: first match.
  default: next match*/
void *region_alloc(mem_regions *regions, size_t nbytes, int match_type);

/* Split an existing range at nbytes, reassigning "next" property
  so that the order of memory regions is preserved */
void split_regions(region_range *matched_region, size_t nbytes);

/* Mark "range_to_free" as unreserved.
  If region is now adjacent to other unreserved regions, merge them */
void region_free(mem_regions *regions, void *addr_to_free);

/* Append a region_range struct to the end of a mem_regions list */
void add_region(region_range *region, mem_regions *regions);

/* Remove all regions that match (that return EQ using cmp_region function) from
 * list */
region_range *remove_region(mem_regions *regions,
                            region_range *region_to_remove);

/* Find region that has start_addr == addr_to_find */
region_range *find_region(mem_regions *regions, void *addr_to_find);

/* Get the nth region (or NULL if n > list length) in list */
region_range *get_nth_region(mem_regions *regions, size_t n);

/* Compares two region_range structs.
   Returns: GT(1) if the lower part of the range of r1 is greater than r2.
   Returns: EQ(0) if the lower part of r1 matches that of r2.
   Returns: LT(-1) if the lower part of r1 is less than that of r2. */
Ord cmp_region(region_range *r1, region_range *r2);
