# TODO: windows support

CXXHOST ?= $(CXX)
CXX := $(CXXHOST)

map_assembler: map_assembler.o
	$(CXXHOST) $(LDFLAGS) $? -o $@

.PHONY: clean
clean:
	rm -f map_assembler.o map_assembler
