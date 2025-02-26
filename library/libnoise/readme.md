# LibNoise



## Presentation

Libnoise is an open-source C++ library for generating coherent 2D procedural maps.

Official site is hosted by [SourceForge](http://libnoise.sourceforge.net/)

## 

## Documentation

Libnoise doxygen documentation is included in the /doc repository.



## Installing libnoise

### Make

```
CXXFLAGS='-O3 -fomit-frame-pointer ...options of choice...' make
```

Using compiler optimizations for libnoise is **strongly recommended**.  Using the unoptimized library is roughly a fifth as fast as using -O3 on my test computer.

### Install (Linux)

The **/include** directory should be copied to your software include directory, usually `/usr/local/include`, with an appropriate name.  I use:

```
cp -R include /usr/local/include/noise
```

Similarly, the *contents* of the **/lib** directory should go into your library directory (usually `/usr/local/lib`).  Once you've done that, run **ldconfig**, then make a *symlink* from `LIBDIR/libnoise.so`  to `LIBDIR/libnoise.so.0`.  I use:
```bash
cp lib/* /usr/local/lib
ldconfig
ln -s /usr/local/lib/libnoise.so.0 /usr/local/lib/libnoise.so
```

At this point, you should be able to include **libnoise** in your projects using:
```c
#include <noise/noise.h>
```
and
```
g++ ...options... -lnoise
```

At some point we'll automate this process.
