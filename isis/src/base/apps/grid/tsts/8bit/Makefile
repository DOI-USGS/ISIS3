APPNAME = grid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/8bit.cub \
	to=$(OUTPUT)/grid.cub mode=ground outline=yes \
	linewidth=3 linevalue=dn dnvalue=255 > /dev/null;
