# mremap

## SYNOPSIS
```c
void *mremap(void *old_address, size_t old_size,
                    size_t new_size, int flags, ... /* void *new_address */);
```

## DESCRIPTION
`mremap()` expands (or shrinks) an existing memory mapping, potentially moving it at the same time (controlled by the flags argument and the available virtual address space).

_old_address_  is the old address of the virtual memory block that you want to expand (or shrink).  Note that _old_address_ has to be page aligned.  _old_size_ is the old size of the virtual memory block. _new_size_ is the requested size of the virtual memory block after the resize.  An optional fifth argument, _new_address_, may be provided; see the description of MREMAP_FIXED below.

In Linux the memory is divided into pages.  A user process has (one or) several linear virtual memory segments.  Each virtual memory segment has one or more mappings to real  memory  pages  (in  the page  table).   Each  virtual memory segment has its own protection (access rights), which may cause a segmentation violation if the memory is accessed incorrectly (e.g., writing to a read-only segment).  Accessing virtual memory outside of the segments will also cause a segmentation violation.

`mremap()` uses the Linux page table scheme.  `mremap()` changes the mapping between virtual addresses and memory pages.  This can be used to implement a very efficient realloc(3).

## KERNEL IMPLEMENTATION

**mm/mremap.c**

```c
SYSCALL_DEFINE5(mremap, unsigned long, addr, unsigned long, old_len,
                unsigned long, new_len, unsigned long, flags,
                unsigned long, new_addr)
```

There are 4 scenarios in this function:
- **REMAP** : userspace specified **new_addr** argument and **MREMAP_FIXED**, use `mremap_to()`.
- **SHRINK**: `old_len >= new_len` means to shrink, that just uses `do_munmap()` to unmap the unnecessary pages.
- **EXPAND**: need to expand, use `vma_adjust()`.
- **MOVE**  : can't expand, we need to create a new one and move it.

At last, if new_len > old_len, we need to call `mm_populate()`

### REMAP
### SHRINK
Simply call `do_munmap()` function.

```c
int do_munmap(struct mm_struct *mm, unsigned long start, size_t len, struct list_head *uf)
```

**mm** is the **memory descriptor** that stores all information related to the process address space.

Refer to [munmap()](./munmap.md)

### EXPAND
### MOVE
### POPULATE



