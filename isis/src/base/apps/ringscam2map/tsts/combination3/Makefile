# This case tests the following combination of parameters for ringscam2map
#
# pixres=ppd resolution=23.695644520884
# defaultrange=map
# minrad=87899725.492461
# maxrad=165123259.31231
# minaz=-179.98841362695
# maxaz=179.98960224884
# azseam=continue
#
APPNAME = ringscam2map


issWAC.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/W1591159850_1_cal.cub \
	  map=$(INPUT)/planar.map \
	  to=$(OUTPUT)/issWAC.cub \
	  pixres=ppd \
	  resolution=23.695644520884 \
	  defaultrange=map \
	  minrad=87899725.492461 \
	  maxrad=165123259.31231 \
	  minaz=-179.98841362695 \
	  maxaz=179.98960224884  \
	  > /dev/null;
	catlab from=$(OUTPUT)/issWAC.cub \
	  to=$(OUTPUT)/issWAC.pvl > /dev/null;
