APPNAME = makeflat

include $(ISISROOT)/make/isismake.tsts

commands:
	$(ECHO) $(INPUT)/ctx_pmoi_i_00003.bottom.cub > $(OUTPUT)/fromList.lis;
	$(ECHO) $(INPUT)/ctx_pmoi_i_00003.top.cub >> $(OUTPUT)/fromList.lis;
	$(APPNAME) fromlist=$(OUTPUT)/fromList.lis \
	to=$(OUTPUT)/pushFrameFlatTruth.cub STDEV=150 \
	IMAGETYPE=PUSHFRAME FRAMELETHEIGHT=50 EXCLUDE=$(OUTPUT)/pushFrame.pvl \
	> /dev/null;
	$(RM) $(OUTPUT)/fromList.lis
