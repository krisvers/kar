CC = clang
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2 -Ikar/include
DYLIB_EXT = so
DYLIB_PRE = lib
DYLIB_FLAGS = -shared -fPIC

kar-dynamic:
	$(CC) kar/src/kar.c -o $(DYLIB_PRE)kar.$(DYLIB_EXT) $(CFLAGS) $(DYLIB_FLAGS)
