APPNAME = mdis2pds

output.cub.TOLERANCE = 0.0000000002
output.pvl.IGNORELINES = PRODUCT_CREATION_TIME
#NOPREFERENCES=

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0219647590M.cal.cub \
	to=$(OUTPUT)/output > /dev/null;
	catlab from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output > /dev/null;
	$(RM) $(OUTPUT)/output.IMG;
