APPNAME = ckwriter

include $(ISISROOT)/make/isismake.tsts

#  Only use 12 digits in the output of tabledump due to precision issues
commands:
	$(APPNAME) FROM=$(INPUT)/I07256002RDR_band9.cub \
	TO=$(OUTPUT)/I07256002RDR_band9.bc > /dev/null;
	$(CP) $(INPUT)/I07256002RDR_band9.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/I07256002RDR_band9.cub \
	  ck=$(OUTPUT)/I07256002RDR_band9.bc > /dev/null;
	tabledump from=$(OUTPUT)/I07256002RDR_band9.cub \
	  to=$(OUTPUT)/I07256002RDR_band9.dat \
	  name=InstrumentPointing > /dev/null;
	cat $(OUTPUT)/I07256002RDR_band9.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/I07256002RDR_band9.txt
	$(RM) $(OUTPUT)/I07256002RDR_band9.cub 
	$(RM) $(OUTPUT)/I07256002RDR_band9.bc
	$(RM) $(OUTPUT)/I07256002RDR_band9.dat


