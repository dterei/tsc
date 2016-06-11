// Support for using the TSC register on intel machines as a timing method.
//
// Should compile with -O to ensure inline attribute is honoured.
//

#ifndef __TSC_HDR
#define __TSC_HDR

#include <stdint.h>

#define TSC_OVERHEAD_N 100000

static inline uint64_t bench_start(void)
{
  unsigned  cycles_low, cycles_high;
  uint64_t t;

  asm volatile( "CPUID\n\t" // serialize
                "RDTSC\n\t" // read clock
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                : "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx" );
  return ((uint64_t) cycles_high << 32) | cycles_low;
}

static inline uint64_t bench_end(void)
{
  unsigned  cycles_low, cycles_high;
  uint64_t t;

  asm volatile( "RDTSCP\n\t" // read clock + serialize
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                "CPUID\n\t" // serialze -- but outside clock region!
                : "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx" );
  return ((uint64_t) cycles_high << 32) | cycles_low;
}

static uint64_t measure_tsc_overhead(void)
{
  uint64_t t0, t1, overhead = ~0;
  int i;

  for (i = 0; i < TSC_OVERHEAD_N; i++) {
    t0 = bench_start();
    asm volatile("");
    t1 = bench_end();
    if (t1 - t0 < overhead)
      overhead = t1 - t0;
  }

  return overhead;
}

#endif /* __TSC_HDR */
