# This case tests the following default parameters for ringscam2map
#
# matchmap=false
# pixres=camera
# defaultrange=minimize
# ringlonseam=auto
# interp=cubicconvolution
# warpalgorithm=automatic
#
APPNAME = ringscam2map

vimsVIS.cub.TOLERANCE = .000062

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/v1496720128_1_vis_cal.cub \
	  map=$(INPUT)/planar.map \
	  to=$(OUTPUT)/vimsVIS.cub > /dev/null;
	catlab from=$(OUTPUT)/vimsVIS.cub \
	  to=$(OUTPUT)/vimsVIS.pvl > /dev/null;
