# Specify HOST to cross compile
# HOST = i686-w64-mingw32

CC = gcc
LD = gcc

CFLAGS += -std=c99 -Wall -Wextra

ifneq ($(HOST),)
CC := $(HOST)-$(CC)
LD := $(HOST)-$(LD)
endif

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
SOURCES += randgen_win32.c winconsole.c
else
E =
SOURCES += randgen_unix.c
endif

OBJECTS = $(addprefix $(HOST)_,$(SOURCES:%.c=%.o))

$(HOST)_%.o: %.c
	$(COMPILE.c) -o $@ $<

diceware$(E): $(OBJECTS)
	$(LINK.c) -o $@ $^

.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) diceware$(E)

ifeq ($(BUILD_WIN32),)
PACKAGEVER = $(DICEWARE_VERSION)
else
PACKAGEVER = $(DICEWARE_VERSION)-win32
endif

.PHONY: package
package:
	zip diceware-$(PACKAGEVER).zip diceware$(E)
