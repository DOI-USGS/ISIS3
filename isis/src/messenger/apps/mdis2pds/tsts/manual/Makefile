# Tests mdis2pds with user-defined (manual) stretch of -1 to 1.
#
# parameters  type=manual, 
#             min = -1, 
#             max =  1, 
# defaults to minper=0, 
#             maxper=100, 
#             bits=32
APPNAME = mdis2pds

output.cub.TOLERANCE = 0.0000000002
output.pvl.IGNORELINES = PRODUCT_CREATION_TIME
#NOPREFERENCES=

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0108840044M.cal.cub \
	to=$(OUTPUT)/output type=manual \
	min=-1.0 max=1.0 > /dev/null;
	catlab from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output.IMG \
	to=$(OUTPUT)/output > /dev/null;
	$(RM) $(OUTPUT)/output.IMG;
