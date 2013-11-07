# This case tests the following combination of parameters for ringscam2map
#
# map=*.lev2.cub
# pixres=map
# defaultrange=camera
# ringlonseam=error
# trim=yes
# interp=bilinear
# warpalgorithm=reversepatch patch=5
#
APPNAME = ringscam2map

vimsIR.cub.TOLERANCE = .000062

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/v1496720128_1_ir_cal.cub \
	  map=$(INPUT)/v1496720128_1_ir_cal.lev2.cub \
	  to=$(OUTPUT)/vimsIR.cub \
	  pixres=map \
	  defaultrange=camera \
	  ringlonseam=error \
	  trim=yes \
	  interp=bilinear \
	  warp=reverse \
	  patch=4 \
	  > /dev/null;
	catlab from=$(OUTPUT)/vimsIR.cub \
	  to=$(OUTPUT)/vimsIR.pvl > /dev/null;
