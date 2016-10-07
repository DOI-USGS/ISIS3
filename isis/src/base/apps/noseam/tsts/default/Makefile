APPNAME = noseam

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/* > $(OUTPUT)/cubes.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/cubes.lis \
	TO=$(OUTPUT)/result.cub \
	SAMPLES=73 LINES=73 > /dev/null;
	$(RM) $(OUTPUT)/cubes.lis;
