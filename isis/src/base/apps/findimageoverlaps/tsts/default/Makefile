APPNAME = findimageoverlaps

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cubes.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/cubes.lis OVERLAPLIST=$(OUTPUT)/overlaps.txt > /dev/null;
	$(RM) $(OUTPUT)/cubes.lis;
