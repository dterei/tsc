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

Sadly, the only way to determine the TSC frequency appears to be through a MSR
using the `rdmsr` instruction. This instruction is privileged and can't be
executed from user-space.

If we could, we want to access the `MSR_PLATFORM_INFO`:

> Register Name: MSR_PLATFORM_INFO [15:8]
> Description: Package Maximum Non-Turbo Ratio (R/O)
>              The is the ratio of the frequency that invariant TSC runs at.
>              Frequency = ratio * 100 MHz.

The multiplicative factor of `100 MHz` varies across architectures. Luckily, it
appears to be `100 MHz` on all Intel architectures except Nehalem, for which it
is `133.3 MHz`.

If this method fails or is unavailable, Linux appears to determine the TSC
clock speed through a [calibration] [lxr3] against hardware timers.

For now, we don't provide the ability to convert cycles to time.

## Intel TSC Documentation

Using `CPUID`

> CPUID.01H : ECX.SSE[bit 25] = 1
> Issue: `CPUID` with value `01H` in register `EAX`
> Output: Value in `ECX`, check bit `25` is `1` indicating `SSE` support

For example:

        static inline bool check_rdtscp(void)
        {
          unsigned cpu;
          asm("mov $80000001H, %%rax\n\t"
              "cpuid\n\t"
              : "=d"(cpu)
              :: "%rax", "%rdx"
              );
          return ( cpu & (1<<27) );
        }

TSC Flag:

> CPUID.1:EDX.TSC[bit 4] = 1.

Invariant TSC:

> The time stamp counter in newer processors may support an enhancement,
> referred to as invariant TSC.  Processor’s support for invariant TSC is
> indicated by CPUID.80000007H:EDX[8].  The invariant TSC will run at a
> constant rate in all ACPI P-, C-. and T-states.  This is the architectural
> behavior moving forward.  On processors with invariant TSC support, the OS
> may use the TSC for wall clock timer services (instead of ACPI or HPET
> timers). TSC reads are much more efficient and do not incur the overhead
> associated with a ring transition or access to a platform resource.

`IA32_TSC_AUX` Register and `RDTSCP` Support:

> Processors based on Intel microarchitecture code name Nehalem provide an
> auxiliary TSC register, IA32_TSC_AUX that is designed to be used in conjunction
> with IA32_TSC. IA32_TSC_AUX provides a 32-bit field that is initialized by
> privileged software with a signature value (for example, a logical processor
> ID).  The primary usage of IA32_TSC_AUX in conjunction with IA32_TSC is to
> allow software to read the 64-bit time stamp in IA32_TSC and signature value in
> IA32_TSC_AUX with the instruction RDTSCP in an atomic operation.  RDTSCP
> returns the 64-bit time stamp in EDX:EAX and the 32-bit TSC_AUX signature value
> in ECX. The atomicity of RDTSCP ensures that no context switch can occur
> between the reads of the TSC and TSC_AUX values.  Support for RDTSCP is
> indicated by CPUID.80000001H:EDX[27]. As with RDTSC instruction, non-ring 0
> access is controlled by CR4.TSD (Time Stamp Disable flag).  User mode software
> can use RDTSCP to detect if CPU migration has occurred between successive reads
> of the TSC.  It can also be used to adjust for per-CPU differences in TSC
> values in a NUMA system.

Invariant Time Keeping:

> The invariant TSC is based on the invariant timekeeping hardware (called
> Always Running Timer or ART), that runs at the core crystal clock frequency.
> The ratio defined by CPUID leaf 15H expresses the frequency relationship
> between the ART hardware and TSC.  If CPUID.15H:EBX[31:0] != 0 and
> CPUID.80000007H:EDX[InvariantTSC] = 1, the following linearity relationship
> holds between TSC and the ART hardware: TSC_Value = (ART_Value *
> CPUID.15H:EBX[31:0] )/ CPUID.15H:EAX[31:0] + K Where 'K' is an offset that
> can be adjusted by a privileged agent2.  When ART hardware is reset, both
> invariant TSC and K are also reset.

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

