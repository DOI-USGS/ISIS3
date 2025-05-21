# This case tests the following combination of parameters for ringscam2map
#
# pixres=ppd resolution=23.7
# defaultrange=map
# minringrad=87899725.492461
# maxringrad=165123259.31231
# minringlon=-180.0
# maxringlon=-139.0
# interp=nearestneighbor \
#
APPNAME = ringscam2map

# The cubes have values on the order of 176663338638.330, which needs a large tolerance.
issWAC.cub.TOLERANCE = 80000

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/W1591159850_1_cal.cub \
	  map=$(INPUT)/planar.map \
	  to=$(OUTPUT)/issWAC.cub \
	  pixres=ppd \
	  resolution=23.7 \
	  defaultrange=map \
	  minringrad=87899725.492461 \
	  maxringrad=165123259.31231 \
	  minringlon=-180.0 \
	  maxringlon=-139.0  \
	  interp=nearestneighbor \
	  > /dev/null;
	catlab from=$(OUTPUT)/issWAC.cub \
	  to=$(OUTPUT)/issWAC.pvl > /dev/null;
