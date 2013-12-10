# This case tests the following combination of parameters for ringscam2map
#
# pixres=cam and map file has Scale, but no PixelResolution
# defaultrange=camera
# ringlonseam=auto
# warpalgorithm=forward patch=1
#
# pixres=mpp and map file has Scale
# defaultrange=minimize
# ringlonseam=continue
# warpalgorithm=reverse patch=3
# 
# pixres=ppd and map file does not have PixelResolution
#
APPNAME = ringscam2map

vimsVIS_1.cub.TOLERANCE = .000001
vimsVIS_2.cub.TOLERANCE = 1.6649
vimsVIS_3.cub.TOLERANCE = .6508

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/v1496720128_1_vis_cal.cub \
	  map=$(INPUT)/planar.map \
	  to=$(OUTPUT)/vimsVIS_1.cub \
	  pixres=cam \
	  defaultrange=camera \
	  ringlonseam=auto \
	  warp=forward \
	  patch=1 \
	  > /dev/null;
	$(APPNAME) from= $(INPUT)/v1496720128_1_vis_cal.cub \
	  map=$(INPUT)/planar.map \
	  to=$(OUTPUT)/vimsVIS_2.cub \
	  pixres=mpp \
	  resolution=646251.0 \
	  defaultrange=minimize \
	  ringlonseam=continue \
	  warp=reverse \
	  patch=3 \
	  > /dev/null;
	$(APPNAME) from= $(INPUT)/v1496720128_1_vis_cal.cub \
	  map=$(INPUT)/planar.map \
	  to=$(OUTPUT)/vimsVIS_3.cub \
	  pixres=ppd \
	  resolution=4 \
	  > /dev/null;
	catlab from=$(OUTPUT)/vimsVIS_1.cub \
	  to=$(OUTPUT)/vimsVIS_1.pvl > /dev/null;
	catlab from=$(OUTPUT)/vimsVIS_2.cub \
	  to=$(OUTPUT)/vimsVIS_2.pvl > /dev/null;
	catlab from=$(OUTPUT)/vimsVIS_3.cub \
	  to=$(OUTPUT)/vimsVIS_3.pvl > /dev/null;
