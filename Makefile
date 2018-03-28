
MODE = zero

include Makefile.in

### specify the target OS
OS = _LINUX_
COPTS += -D$(OS)

### directories where include files are found
INCLUDE = -I .

### -rpath arguments for finding .so objects in pre-specified locations
MY_DIR = .
RPATH = -rpath=$(MY_DIR)

### linking options
LOPTS += -L . -lINutils

### adding any debugging info
#COPTS += -D_DEBUG1_
#COPTS += -D_DEBUG2_
#COPTS += -D_DEBUG3_


code: libs
	$(CC) -c $(COPTS) $(INCLUDE) inTetBlock.c
	$(CC) -c $(COPTS) $(INCLUDE) main.c -L . -lINutils
	$(CC) $(LOPTS) -Wl,$(RPATH) main.o inTetBlock.o


libs:
	$(CC) -c $(COPTS) inPlot3d.c
	$(CC) -shared -Wl,-soname,libINutils.so  -o libINutils.so inPlot3d.o -lc 


clean:
	rm -f  *.o a.out libIN*.so

