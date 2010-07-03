APPNAME = findimageoverlaps

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cubes.lis;
	$(APPNAME) FROM=$(OUTPUT)/cubes.lis TO=$(OUTPUT)/overlaps.txt > /dev/null;
	$(RM) $(OUTPUT)/cubes.lis;
