# Orbit tests for mdis2pds
#
# defaults to type=automatic, 
#             min = -1, 
#             max =  1, 
#             minper=0, 
#             maxper=100, 
#             bits=32
APPNAME = mdis2pds

output.cub.TOLERANCE = 0.0000000002
output.pvl.IGNORELINES = PRODUCT_CREATION_TIME
#NOPREFERENCES=

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EW0216154583G.cal.cub \
	to=$(OUTPUT)/output > /dev/null;
	catlab from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output > /dev/null;
	$(RM) $(OUTPUT)/output.IMG;
