APPNAME = spkwriter

include $(ISISROOT)/make/isismake.tsts

#  Only use 12 digits in the output of tabledump due to precision issues
commands:
	$(APPNAME) FROM=$(INPUT)/EW1025478282G.lev1.cub \
	TO=$(OUTPUT)/EW1025478282G.bsp > /dev/null;
	$(CP) $(INPUT)/EW1025478282G.lev1.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/EW1025478282G.lev1.cub \
	 spk=$(OUTPUT)/EW1025478282G.bsp > /dev/null;
	tabledump from=$(OUTPUT)/EW1025478282G.lev1.cub \
	  to=$(OUTPUT)/EW1025478282G.txt \
	  name=InstrumentPosition > /dev/null;
#	cat $(OUTPUT)/EW1025478282G.dat \
#	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
#	  > $(OUTPUT)/EW1025478282G.txt;
	$(RM) $(OUTPUT)/EW1025478282G.lev1.cub 
	$(RM) $(OUTPUT)/EW1025478282G.bsp


