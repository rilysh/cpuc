## cpuc
Print information about your CPU's supported extensions.


## Info
CPU-supported instructions are checked by executing them. If you're using a very old version of the Linux kernel, it may not support these extensions. Likewise, using a very new generation of hardware with a very old Linux kernel would have the same result. Overall, several features, need to be enabled at the kernel level; however, if your distribution have GCC 5+ you're good to go.


If you're using a PAE kernel, detecting architecture may return an incorrect value.

## Build
```sh
git clone https://github.com/rilysh/cpuc
cd cpuc
gcc cpuc.c -s -Os -o cpuc
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

