# Site ID test for mdis2pds
#
# defaults to type=automatic, 
#             default min, 
#             default max, 
#             minper=0, 
#             maxper=100, 
#             bits=32
APPNAME = mdis2pds

output.cub.TOLERANCE = 0.0000000002
output.pvl.IGNORELINES = PRODUCT_CREATION_TIME STANDARD_DEVIATION
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
