APPNAME = spkwriter

include $(ISISROOT)/make/isismake.tsts

#  Only use 12 digits in the output of tabledump due to precision issues
commands:
	$(APPNAME) FROM=$(INPUT)/0027399.cub \
	TO=$(OUTPUT)/0027399.bsp > /dev/null;
	$(CP) $(INPUT)/0027399.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/0027399.cub \
	 spk=$(OUTPUT)/0027399.bsp > /dev/null;
	tabledump from=$(OUTPUT)/0027399.cub \
	  to=$(OUTPUT)/0027399.dat \
	  name=InstrumentPosition > /dev/null;
	cat $(OUTPUT)/0027399.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/0027399.txt;
	$(RM) $(OUTPUT)/0027399.cub 
	$(RM) $(OUTPUT)/0027399.bsp
	$(RM) $(OUTPUT)/0027399.dat


