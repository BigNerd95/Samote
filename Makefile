LOADER := loader
SAMOTE := samote

SRCDIR := src
INC := include
BUILDDIR := build

BINLOADER := bin/$(LOADER).so
SRCLOADER := $(shell find $(LOADER)/$(SRCDIR) -type f -name "*.c")
OBJLOADER := $(patsubst $(LOADER)/$(SRCDIR)/%,$(BUILDDIR)/$(LOADER)/%,$(SRCLOADER:.c=.o)) 

BINSAMOTE := bin/$(SAMOTE).so
SRCSAMOTE := $(shell find $(SAMOTE)/$(SRCDIR) -type f -name "*.c")
OBJSAMOTE := $(patsubst $(SAMOTE)/$(SRCDIR)/%,$(BUILDDIR)/$(SAMOTE)/%,$(SRCSAMOTE:.c=.o)) 

LDFLAGS=-shared -Wl -pthread
CFLAGS=-c -O2
CC=../../SamyGO_toolchain-armv5_armv6/bin/arm_v6_vfp_le-gcc


all: $(LOADER) $(SAMOTE)

$(LOADER): $(SRCLOADER) $(BINLOADER)

$(SAMOTE): $(SRCSAMOTE) $(BINSAMOTE)



$(BINLOADER): $(OBJLOADER)
	$(CC) $(LDFLAGS) $(OBJLOADER) -o $@

$(BINSAMOTE): $(OBJSAMOTE)
	$(CC) $(LDFLAGS) $(OBJSAMOTE) -o $@



$(BUILDDIR)/$(SAMOTE)/%.o: $(SAMOTE)/$(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)/$(SAMOTE)
	$(CC) $(CFLAGS) -I $(SAMOTE)/$(INC) $< -o $@

$(BUILDDIR)/$(LOADER)/%.o: $(LOADER)/$(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)/$(LOADER)
	$(CC) $(CFLAGS) -I $(LOADER)/$(INC) $< -o $@

clean:
	rm -r $(BUILDDIR) $(BINLOADER) $(BINSAMOTE)
