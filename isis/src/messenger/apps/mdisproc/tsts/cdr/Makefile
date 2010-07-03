APPNAME = mdisproc

include $(ISISROOT)/make/isismake.tsts

output.pvl.IGNORELINES = PRODUCT_CREATION_TIME DARK_STRIP_MEAN MEAN MINIMUM MAXIMUM STANDARD_DEVIATION

commands:
	$(APPNAME) from=$(INPUT)/EN0108840044M.IMG \
	to=$(OUTPUT)/output cdr=true > /dev/null;
	catlab from=$(OUTPUT)/output.IMG to=$(OUTPUT)/output.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output.IMG to=$(OUTPUT)/output.cub > /dev/null;
	$(RM) $(OUTPUT)/output.IMG;
