# O.I.S.

Cross Platform **Object Oriented Input Lib System**. 
Meant to be very robust and compatible with many systems and operating systems

----

## Win32/

- Contains **Visual Studio .Net** Solution Files
- Contains **gcc CodeBlocks** target build project (*Not Working Yet*)

### Dependencies
- DirectInput 8
- Ogre & CEGUI 0.4.0 (if building CEGUIOgre OIS Demo)

----

## Linux/

### Dependencies

  - X11
  - Ogre (--with-platform=GLX) & CEGUI 0.4.0 If building CEGUIOgre OIS Demo
  - Newer Linux Kernel (2.6+ ?) for Event API - else, use `--disable-joyevents`

### Steps to build on Linux

```
	./bootstrap
	./configure
	./make && make install
```

### Configure build options

```
	./configure --help              --- List all configure options
	./configure --disable-ogre      --- Disables CEGUIOgre ActionMapping Demo
	./configure --disable-joyevents --- Uses /dev/input/jsX instead of /dev/input/eventX
```
