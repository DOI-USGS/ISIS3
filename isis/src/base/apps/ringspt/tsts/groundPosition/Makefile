# Test for ringspt reverse projection (Ground values entered by user)
# 
# The first run is a level 1 cube that also tests the following combination of 
# parameters:
#     TO is user entered, FORMAT=PVL, and APPEND=YES for new results file
#
# The second run is a level 2 cube that also tests the following combinations of 
# parameters:
#     TO is user entered, FORMAT=PVL, and APPEND=YES for existing results file
#
APPNAME = ringspt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub \
	  to=$(OUTPUT)/sample300_line300_lev1_reverseproj.pvl \
	  format=pvl \
	  append=yes \
	  type=ground \
	  ringradius=118886467.76208 \
	  ringlongitude=206.19967330028 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/sample1024_line1024_lev2_reverseproj.pvl \
	  format=pvl \
	  append=yes \
	  type=ground \
	  ringradius=102202085.4541 \
	  ringlongitude=234.92853597625 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/negativeSampleAndLine.pvl \
	  format=pvl \
	  append=yes \
	  sample=-1 \
	  line=-1 \
	  allowoutside=yes > /dev/null;
