APPNAME = makeflat

include $(ISISROOT)/make/isismake.tsts

commands:
	$(ECHO) $(INPUT)/ctx_pmoi_i_00003.bottom.cub > $(OUTPUT)/fromList.lis;
	$(ECHO) $(INPUT)/ctx_pmoi_i_00003.top.cub >> $(OUTPUT)/fromList.lis;
	$(APPNAME) fromlist=$(OUTPUT)/fromList.lis \
	  to=$(OUTPUT)/framingFlatTruth.cub STDEV=190 \
	  IMAGETYPE=FRAMING EXCLUDE=$(OUTPUT)/framingFlatTruth.txt> /dev/null;
	$(RM) $(OUTPUT)/fromList.lis
