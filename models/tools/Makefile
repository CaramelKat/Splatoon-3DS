# TODO: windows support

CXXHOST ?= $(CXX)
CXX := $(CXXHOST)

obj_convert: obj_convert.o
	$(CXXHOST) $(LDFLAGS) $? -o $@

.PHONY: clean
clean:
	rm -f obj_convert.o obj_convert
