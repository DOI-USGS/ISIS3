APPNAME = ckwriter
BASES    = $(basename $(notdir $(wildcard $(INPUT)/*.cub)))
EXTRA    = $(shell ls -1t $(ISIS3DATA)/messenger/kernels/ck/*_mdis_pivot_pvtres.bc | head -1)


include $(ISISROOT)/make/isismake.tsts

.PHONY: commands

commands: $(BASES)
	$(RM) $(OUTPUT)/messyck.bc

#  Only use 12 digits in the output of tabledump due to precision issues
$(BASES): $(OUTPUT)/messyck.bc
	$(CP) $(INPUT)/$@.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/$@.cub ck=$(OUTPUT)/messyck.bc extra=$(EXTRA)  > /dev/null; 
	tabledump from=$(OUTPUT)/$@.cub to=$(OUTPUT)/$@.dat \
	  name=InstrumentPointing  > /dev/null;
	cat $(OUTPUT)/$@.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/$@.txt;
	$(RM) $(OUTPUT)/$@.cub 
	$(RM) $(OUTPUT)/$@.dat

ck: $(OUTPUT)/messyck.bc

$(OUTPUT)/messyck.bc:
	mkdir -p $(OUTPUT);
	ls -1 $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/files.lis TO=$@  > /dev/null;
	$(RM) $(OUTPUT)/files.lis


