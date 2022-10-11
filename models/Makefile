# hot take: devkitPro makefiles are massively overcomplicated
# I still prefer CMake
.PHONY: all tools clean

MODELS := $(wildcard *.obj)
3MDLS := $(MODELS:.obj=.3mdl)

# all must be the first target in the file, because make
all: $(3MDLS)

tools:
	@$(MAKE) -C tools

clean:
	@$(MAKE) -C tools clean
	rm -f $(3MDLS)

ccyellow=$(shell echo -e "\033[0;33m")
ccend=$(shell echo -e "\033[0m")

%.3mdl: %.obj tools
	@echo -e "$(ccyellow)convert$(ccend): $< -> $@"
	@tools/obj_convert $< $@
