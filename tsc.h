#ifndef TSC_H
#define TSC_H

/**
 * tsc.h - support for using the TSC register on intel machines as a timing
 * method. Should compile with -O to ensure inline attribute is honoured.
 *
 * author: David Terei <code@davidterei.com>
 * copyright: Copyright (c) 2016, David Terei
 * license: BSD
 */

#include <stdint.h>

#define TSC_OVERHEAD_N 100000

// bench_start returns a timestamp for use to measure the start of a benchmark
// run.
static inline uint64_t bench_start(void)
{
  unsigned  cycles_low, cycles_high;
  asm volatile( "CPUID\n\t" // serialize
                "RDTSC\n\t" // read clock
                "MOV %%edx, %0\n\t"
                "MOV %%eax, %1\n\t"
                : "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx" );
  return ((uint64_t) cycles_high << 32) | cycles_low;
}

// bench_end returns a timestamp for use to measure the end of a benchmark run.
static inline uint64_t bench_end(void)
{
  unsigned  cycles_low, cycles_high;
  asm volatile( "RDTSCP\n\t" // read clock + serialize
                "MOV %%edx, %0\n\t"
                "MOV %%eax, %1\n\t"
                "CPUID\n\t" // serialze -- but outside clock region!
                : "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx" );
  return ((uint64_t) cycles_high << 32) | cycles_low;
}

// measure_tsc_overhead returns the overhead from benchmarking, it should be
// subtracted from timings to improve accuracy.
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

#endif /* TSC_H */
