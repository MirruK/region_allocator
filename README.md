# Memory management scheme

## Idea: 

For some region of free memory from byte number 0..n,
allow the user to reserve a region of that memory, 
this records the section of memory allocated and stores
it in a linked list of ranges of bytes that describe
which regions of memory (start-addr., end-addr.) are
free or reserved.

## Extra: 

Implement some sort of defragmentation to maximise chance
of quickly finding an adequately sized region of memory.

## Extra extra:

Implement multiple schemes for allocating new memory regions
For example:

    1. First fit

    2. Next fit

    3. Best fit

    4. Worst fit

    5. Quick fit

### Notes

Keep track of holes of unallocated memory in a sorted list (largest hole first).
Quickly find best fit for the amount of memory that is requested.

## Resources:
[https://www.geeksforgeeks.org/memory-allocation-techniques-mapping-virtual-addresses-to-physical-addresses/]
