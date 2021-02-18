APPNAME = makeflat

linescanFlatTruth.cub.TOLERANCE = 0.0000002

include $(ISISROOT)/make/isismake.tsts

commands:
	$(ECHO) $(INPUT)/ctx_pmoi_i_00003.bottom.cub > $(OUTPUT)/fromList.lis;
	$(ECHO) $(INPUT)/ctx_pmoi_i_00003.top.cub >> $(OUTPUT)/fromList.lis;
	$(APPNAME) fromlist=$(OUTPUT)/fromList.lis \
	  to=$(OUTPUT)/linescanFlatTruth.cub STDEV=190 \
	  IMAGETYPE=LINESCAN NUMLINES=10 \
	  EXCLUDE=$(OUTPUT)/linescanFlatTruth.txt > /dev/null;
	  $(RM) $(OUTPUT)/fromList.lis
