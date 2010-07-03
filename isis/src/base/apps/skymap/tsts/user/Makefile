APPNAME = skymap
skymapTruth.cub.TOLERANCE = 0.000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to= $(OUTPUT)/skymapTruth.cub \
	  defaultscale=user \
	  scale=0.25 \
	  interp=bilinear> /dev/null;
