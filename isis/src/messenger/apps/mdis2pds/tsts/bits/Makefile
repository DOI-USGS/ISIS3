# Tests BITS parameter for mdis2pds
#
# defaults to type=automatic, 
#             min = -1, 
#             max =  1, 
#             minper = 0, 
#             maxper = 100, 
# test with bits = 8, 12, 16
# test with bits = 4, 64 to test error throws
APPNAME = mdis2pds

output08bit.cub.TOLERANCE = 0.0000000002
output16bit.cub.TOLERANCE = 0.0000000002
output12bit.cub.TOLERANCE = 0.0000000002
#NOPREFERENCES=

include $(ISISROOT)/make/isismake.tsts

commands:
	# 8 bit
	$(APPNAME) from=$(INPUT)/EN0108840044M.cal.cub \
	to=$(OUTPUT)/output08bit \
	bits=8 > /dev/null;
	catlab from=$(OUTPUT)/output08bit.IMG \
	to=$(OUTPUT)/output08bit.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output08bit.IMG \
	to=$(OUTPUT)/output08bit > /dev/null;
	$(RM) $(OUTPUT)/output08bit.IMG;
	# 16 bit
	$(APPNAME) from=$(INPUT)/EN0108840044M.cal.cub \
	to=$(OUTPUT)/output16bit \
	bits=16 > /dev/null;
	catlab from=$(OUTPUT)/output16bit.IMG \
	to=$(OUTPUT)/output16bit.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output16bit.IMG \
	to=$(OUTPUT)/output16bit > /dev/null;
	$(RM) $(OUTPUT)/output16bit.IMG;
	# 12 bit
	$(APPNAME) from=$(INPUT)/EN0108840044M.cal.cub \
	to=$(OUTPUT)/output12bit \
	bits=12 > /dev/null;
	catlab from=$(OUTPUT)/output12bit.IMG \
	to=$(OUTPUT)/output12bit.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/output12bit.IMG \
	to=$(OUTPUT)/output12bit > /dev/null;
	$(RM) $(OUTPUT)/output12bit.IMG;
	# Test for bits between 16 and 32
	if [[ `$(APPNAME) \
	  from=$(INPUT)/EN0108840044M.cal.cub \
	  to=$(OUTPUT)/output04bit \
	  bits=24 \
	  2>> $(OUTPUT)/error.txt` ]]; \
	then \
	  true; \
	fi;
	$(RM) $(OUTPUT)/output04bit.IMG;
