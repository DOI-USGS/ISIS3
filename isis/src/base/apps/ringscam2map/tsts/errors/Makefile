# This case tests the errors thrown by ringscam2map
#
# A. The image is targeting the sky, use skymap instead.
# B. The image crosses the ring longitude (azimuth) seam. (And ringlonseam=error).
# C. The image does not have a plane shape. Run spiceinit with shape=ringplane.
#
# D. The input file is a pushframe camera cropped at a fractional pixel. Can
#    not project.
#
# if helper for LoadMapRes is selected
# E. Neither PixelResolution nor Scale are not found in map file.
#
# if helper for LoadMapRange is selected
# F. One or more of the values for the ground range was not found in map file.
#
.IGNORE:
APPNAME = ringscam2map

vimsVIS.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
#   TEST A: target is sky
	echo -e "Error Test A:" > $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/targetSky.cub \
	map=$(INPUT)/planarCassiniVIMS.map \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST B: crosses ring longitude (azimuth) seam
	echo -e "\nError Test B:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/seam.cub \
	map=$(INPUT)/seam.map \
	to=$(OUTPUT)/output.cub \
	ringlonseam=error 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST C: not ring plane shape
	echo -e "\nError Test C:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/notRingPlane.cub \
	map=$(INPUT)/sinusoidal.map \
	to=$(OUTPUT)/output.cub \
	2>> $(OUTPUT)/error_temp.txt > /dev/null;

	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/error_temp.txt;
	rm -f $(OUTPUT)/output.cub;
