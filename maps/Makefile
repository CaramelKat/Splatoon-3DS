# hot take: devkitPro makefiles are massively overcomplicated
# I still prefer CMake
.PHONY: all tools clean

MAPS := $(wildcard *.map)
3MAPS := $(MAPS:.map=.3map)

# all must be the first target in the file, because make
all: $(3MAPS)

tools:
	@$(MAKE) -C tools

clean:
	@$(MAKE) -C tools clean
	rm -f $(3MAPS)

ccyellow=$(shell echo -e "\033[0;33m")
ccend=$(shell echo -e "\033[0m")

%.3map: %.map tools
	@echo -e "$(ccyellow)convert$(ccend): $< -> $@"
	@tools/map_assembler $< $@
