# This case tests the following combination of parameters for ringscam2map
#
# pixres=mpp resolution=1703.2815626544
# defaultrange=minimize
# ringlonseam=continue
# interp=nearestneighbor
# warpalgorithm=forwardpatch  patch=4
#
APPNAME = ringscam2map

issNAC.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/N1495328431_1_ct.cub \
	  map=$(INPUT)/planar.map \
	  to=$(OUTPUT)/issNAC.cub \
	  pixres=mpp \
	  resolution=1703.2815626544 \
	  ringlonseam=continue \
	  interp=nearestneighbor \
	  warp=forward \
	  patch=4 \
	  > /dev/null;
	catlab from=$(OUTPUT)/issNAC.cub \
	  to=$(OUTPUT)/issNAC.pvl > /dev/null;
