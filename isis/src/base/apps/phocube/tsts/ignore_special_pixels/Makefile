# Tests that special pixels in the input are set to null for all bands.
APPNAME = phocube

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cropped.lvl2.cub \
	  to=$(OUTPUT)/output.cub \
	  specialpixels=false \
	  dn=true \
	  radec=true > /dev/null; 
	catlab from=$(OUTPUT)/output.cub \
	  to=$(OUTPUT)/output.pvl > /dev/null;
