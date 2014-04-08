APPNAME = spkwriter
BASES    = $(basename $(notdir $(wildcard $(INPUT)/*.cub)))


include $(ISISROOT)/make/isismake.tsts

.PHONY: commands

commands: $(BASES)
	$(RM) $(OUTPUT)/apollo15_mixed.bsp

#  Only use 12 digits in the output of tabledump due to precision issues
$(BASES): $(OUTPUT)/apollo15_mixed.bsp
	$(CP) $(INPUT)/$@.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/$@.cub ckrecon=true cksmithed=true spk=$(OUTPUT)/apollo15_mixed.bsp > /dev/null;
	tabledump from=$(OUTPUT)/$@.cub to=$(OUTPUT)/$@.txt \
	  name=InstrumentPosition > /dev/null;
	$(RM) $(OUTPUT)/$@.cub 

spk: $(OUTPUT)/apollo15_mixed.bsp

$(OUTPUT)/apollo15_mixed.bsp:
	mkdir -p $(OUTPUT);
	ls -1 $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/files.lis TO=$@ > /dev/null;
	$(RM) $(OUTPUT)/files.lis


