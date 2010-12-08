APPNAME = ckwriter
BASES    = $(basename $(notdir $(wildcard $(INPUT)/*.cub)))


include $(ISISROOT)/make/isismake.tsts

.PHONY: commands

commands: $(BASES)
	$(RM) $(OUTPUT)/lrock.bc

#  Only use 12 digits in the output of tabledump due to precision issues
$(BASES): $(OUTPUT)/lrock.bc
	$(CP) $(INPUT)/$@.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/$@.cub ck=$(OUTPUT)/lrock.bc > /dev/null;
	tabledump from=$(OUTPUT)/$@.cub to=$(OUTPUT)/$@.dat \
	  name=InstrumentPointing > /dev/null;
	cat $(OUTPUT)/$@.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/$@.txt;
	$(RM) $(OUTPUT)/$@.cub 
	$(RM) $(OUTPUT)/$@.dat

ck: $(OUTPUT)/lrock.bc

$(OUTPUT)/lrock.bc:
	mkdir -p $(OUTPUT);
	ls -1 $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/files.lis TO=$@ > /dev/null;
	$(RM) $(OUTPUT)/files.lis


