# tsc

[![BSD3 License](http://img.shields.io/badge/license-BSD3-brightgreen.svg?style=flat)][tl;dr Legal: BSD3]

[tl;dr Legal: BSD3]:
  https://tldrlegal.com/license/bsd-3-clause-license-(revised)
  "BSD3 License"

C header library for access the CPU timestamp cycle counter (TSC) on x86-64. If
not familar with using the TSC for benchmarking, refer to the
[Intel whitepaper][intel1]. This is designed to be used for benchmarking code,
so takes steps to prevent instruction reordering across measurement boundaries
by the CPU.

## Usage

As a header only library, just include `tsc.h` in your include directory of
your project. Then, use as follows:

``` .c
uint64_t start, end, overhead;

overhead = measure_tsc_overhead();

start = bench_start();
// code to benchmark
end = bench_end();

printf("Execution took %" PRIu64 " clock cycles\n", end - start - overhead);
```

## Reading the TSC

Reading the TSC for benchmarking is a confused subject with differing advice on
how to avoid CPU instruction reordering affecting your measurements. A detailed
write up on the issues can be found at the [gotsc][gotsc] project.

## Converting Cycles to Time

To convert from cycles to wall-clock time we need to know TSC frequency
Frequency scaling on modern Intel chips doesn't affect the TSC.

Sadly, there doesn't seem to be a good way to do this. From Intel V3B, 17.14:

> That rate may be set by the maximum core-clock to bus-clock ratio of the
> processor or may be set by the maximum resolved frequency at which the
> processor is booted. The maximum resolved frequency may differ from the
> processor base frequency, see Section 18.15.5 for more detail. On certain
> processors, the TSC frequency may not be the same as the frequency in the
> brand string.

Linux appears to deteremine the TSC clock speed through a [calibration] [lxr3]
against hardware timers.

However, another project claims that the ratio of TSC clock to bus clock can be
read from `MSR_PLATFORM_INFO[15:8]` (see Intel 64 and IA-32 Architectures
Software Developer’s Manual, Vol. 3C 35-53 (pag. 2852)). Need to check this.

## Licensing

This library is BSD-licensed.

## Get involved!

We are happy to receive bug reports, fixes, documentation enhancements,
and other improvements.

Please report bugs via the
[github issue tracker](http://github.com/dterei/tsc/issues).

Master [git repository](http://github.com/dterei/tsc):

* `git clone git://github.com/dterei/tsc.git`

## Authors

This library is written and maintained by David Terei, <code@davidterei.com>.

[intel1]: http://www.intel.com/content/www/us/en/embedded/training/ia-32-ia-64-benchmark-code-execution-paper.html
[gotsc]: https://github.com/dterei/gotsc

