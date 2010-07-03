APPNAME = cam2cam

cam2cam1.cub.TOLERANCE = .00000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	  match=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/cam2cam1.cub INTERP=BILINEAR > /dev/null;
