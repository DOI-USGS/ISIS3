APPNAME = mdisddr

output.cub.TOLERANCE = 0.0000002
output.pvl.IGNORELINES = PRODUCT_CREATION_TIME

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0131770878M.cal.cub \
	to=$(OUTPUT)/output > /dev/null;
	catlab from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output > /dev/null;
	$(RM) $(OUTPUT)/output.IMG;
