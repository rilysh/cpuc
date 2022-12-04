## cpuc
Print information about your CPU's supported extensions.


## Info
CPU-supported instructions are checked by executing them. If you're using a very old or obscure operating system, chances are program may crash. Regarding your platform, if GCC or Clang have "cpuid.h" header file, means your OS may able to use these features, however, not exact. Definations can be declare manually too. 

Tested on: Void GNU/Linux and FreeBSD\
Note: If you're using a PAE kernel, detecting architecture may return an incorrect value.

## Build
```sh
git clone https://github.com/rilysh/cpuc
cd cpuc
make
```

## Usage
```
CPUC


Usage:
cpuc --all (Prints all detectable CPU extensions)
cpuc --info (Prints detectable general CPU extensions)
cpuc --features (Prints detectable CPU features)
cpuc --simd128 (Prints detectable SIMD 128-bit extension)
cpuc --simd256 (Prints detectable SIMD 256-bit extension)
cpuc --simd512 (Prints detectable SIMD 512-bit extension)
```
