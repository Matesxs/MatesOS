#pragma once

#include "stddef.h"
#include "stdint.h"

#define DIV_ROUNDUP(A, B) ({ \
    __typeof__(A) _a_ = A;       \
    __typeof__(B) _b_ = B;       \
    (_a_ + (_b_ - 1)) / _b_; \
})
#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))
#define	ROUND_UP_PAGE_SIZE(n)	ROUND_UP(n, 4096)
#define	NEAREST_PAGE(n)			(ROUND_UP_PAGE_SIZE(n) / 4096)

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;
extern uint64_t _WritableStart;
extern uint64_t _WritableEnd;
extern uint64_t _virtual_base;

void halt();
uint8_t checksum(const char *addr, size_t size);

#define KERNEL_PHYSICAL_ADDRESS(virtual_address)	((void*)((unsigned long long)(virtual_address) - ((unsigned long long)&_virtual_base)))