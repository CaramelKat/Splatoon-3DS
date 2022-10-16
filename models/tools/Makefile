# TODO: windows support

CXXHOST ?= $(CXX)
CXX := $(CXXHOST)
CXXFLAGS += -std=c++17

obj_convert: obj_convert.o
	$(CXXHOST) $(LDFLAGS) $(CXXFLAGS) $? -o $@

.PHONY: clean
clean:
	rm -f obj_convert.o obj_convert
