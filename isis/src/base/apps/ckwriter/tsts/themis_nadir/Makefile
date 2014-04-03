APPNAME = ckwriter

include $(ISISROOT)/make/isismake.tsts

#  Only use 12 digits in the output of tabledump due to precision issues
commands:
	$(APPNAME) FROM=$(INPUT)/I05055002RDR_band1_nadir.cub \
	TO=$(OUTPUT)/I05055002RDR_band1_nadir.bc > /dev/null;
	$(CP) $(INPUT)/I05055002RDR_band1_nadir.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/I05055002RDR_band1_nadir.cub \
	  ck=$(OUTPUT)/I05055002RDR_band1_nadir.bc > /dev/null;
	tabledump from=$(OUTPUT)/I05055002RDR_band1_nadir.cub \
	  to=$(OUTPUT)/I05055002RDR_band1_nadir.dat \
	  name=InstrumentPointing > /dev/null;
	cat $(OUTPUT)/I05055002RDR_band1_nadir.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/I05055002RDR_band1_nadir.txt
	$(RM) $(OUTPUT)/I05055002RDR_band1_nadir.cub 
	$(RM) $(OUTPUT)/I05055002RDR_band1_nadir.bc
	$(RM) $(OUTPUT)/I05055002RDR_band1_nadir.dat


