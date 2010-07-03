APPNAME = noseam

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/* > $(OUTPUT)/cubes.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/cubes.lis \
	TO=$(OUTPUT)/result.cub \
	HNS=73 HNL=73 LNS=73 LNL=73 > /dev/null;
	$(RM) $(OUTPUT)/cubes.lis;
