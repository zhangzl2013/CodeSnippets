# Page table operations

## <n>level-fixup.h
What do 4level-fixup.h and 5level-fixup.h files do?

Maybe they are used for compatibility with archs that doesn't support so many levels. x86_64 doesn't use it at least.


## page table level in x86_64
In x86_64, it support 4-level paging.

They are:
```
 0000 0000 0  000 0000 00  00 0000 000  0 0000 0000  0000 0000 0000
+------------------------------------------------------------------+
|   9 bits   |   9 bits   |   9 bits   |   9 bits   |    12 bits   | length
+------------------------------------------------------------------+
|47        39|38        30|29        21|20        12|11           0| bit range
+------------------------------------------------------------------+
|    PML4    |   Dir Ptr  |  Directory |    Table   |    Offset    | Name in Intel SDM
+-------------------------------------------------------------------
|   PML4E    |   PDPTE    |    PDE     |    PTE     |    Offset    | Entry name in Intel SDM
+------------------------------------------------------------------+
| p4d_t/pgd_t|   pud_t    |    pmd_t   |    pte_t   |    Offset    | Name in linux kernel
+------------------------------------------------------------------+
```
9 bits can index 512 entries.

```
config PGTABLE_LEVELS
        default 4 if X86_64
        default 3 if X86_PAE
        default 2
```

## page table entry defination
Following is a 5 level pagetable quick reference for x86_64
```c
// x86_64 uses p4d_t defination in include/asm-generic/pgtable-nop4d.h, not in include/asm-generic/5level-fixup.h
typedef unsigned long   pteval_t;
typedef unsigned long   pmdval_t;
typedef unsigned long   pudval_t;
typedef unsigned long   p4dval_t;
typedef unsigned long   pgdval_t;
typedef unsigned long   pgprotval_t;

/* pgd_t: */ typedef struct { pgdval_t pgd; } pgd_t;
/* p4d_t: */ typedef struct { pgd_t    pgd; } p4d_t;    /* for x86_64, pgd_t and p4d_t are the same thing. */
/* pud_t: */ typedef struct { pudval_t pud; } pud_t;
/* pmd_t: */ typedef struct { pmdval_t pmd; } pmd_t;
/* pte_t: */ typedef struct { pteval_t pte; } pte_t;
```

## kernel implementation
### pmd_alloc()

Let's take pmd operations as an example:
For the pmd entry structure, refer to Intel SDM 3 figure 4-11 and table 4-18.
```c
static inline pmd_t *pmd_alloc(struct mm_struct *mm, pud_t *pud, unsigned long address)

```


#### __pmd_alloc()
```c
int __pmd_alloc(struct mm_struct *mm, pud_t *pud, unsigned long address)
```
*pud* is the virtual address of the pud entry.

Its implementation is essentially:

```c
pmd_t *new_pmd = pmd_alloc_one(mm, address);
pud_populate(mm, pud, new_pmd);
```

`pmd_alloc_one()` allocates one page and return its **virtual address**. That is the address of a pmd_t array with 512 items in it.

`pud_populate()` simply put this array's **physical address** to the pud item, OR'ed with __PAGE_TABLE flags.

Now we have the pmd table page, and set its address to its upper pud entry.

We don't need to update any register to inform the CPU about this new entry, because the *page table walking circuit* can access this part of the RAM too, as long as we have put its upper pgd address to CR3.

A figure illustrates more clearly:

```
Before:            After:
  +-----+            +-----+
  | ... |            | ... |
  +-----+            +-----+
  | pud |            | pud | -->  +-----+  # pud contains physical address of pmd array(table)
  +-----+            +-----+      | pmd |
  | ... |            | ... |      +-----+
  +-----+            +-----+      |     |
                                  | ... | <-- pmd_t (1)
                                  |     |
                                  +-----+
```

#### pmd_alloc()
`pmd_alloc()` use `pmd_offset()` to return the **virtual address** of the pmd entry showed in previous figure mark (1)
