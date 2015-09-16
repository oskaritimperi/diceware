# Specify HOST to cross compile
# HOST = i686-w64-mingw32

CC = $(HOST)-gcc
LD = $(HOST)-gcc

CFLAGS += -std=c99 -Wall -g

SOURCES = main.c htable.c

ifneq ($(findstring Windows,$(OS)),)
BUILD_WIN32 = 1
RM = del
else ifneq ($(findstring mingw,$(HOST)),)
BUILD_WIN32 = 1
RM = rm -f
endif

ifneq ($(BUILD_WIN32),)
E = .exe
SOURCES += randgen_win32.c
else
E =
SOURCES += randgen_unix.c
endif

OBJECTS = $(SOURCES:%.c=%.o)

diceware$(E): $(OBJECTS)
	$(LINK.c) -o $@ $^

.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) diceware$(E)
