APPNAME = cam2cam

# 7.5e-09
cam2cam2.cub.TOLERANCE = .0000000075

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	  match=$(INPUT)/ab102402.cub \
	  to=$(OUTPUT)/cam2cam2.cub INTERP=BILINEAR > /dev/null;
