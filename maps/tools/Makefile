# TODO: windows support

CXXHOST ?= $(CXX)
CXX := $(CXXHOST)
CXXFLAGS += -std=gnu++17

map_assembler: map_assembler.o
	$(CXXHOST) $(LDFLAGS) $(CXXFLAGS) $? -o $@

.PHONY: clean
clean:
	rm -f map_assembler.o map_assembler
