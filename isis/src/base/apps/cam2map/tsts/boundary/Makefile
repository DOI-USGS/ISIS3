APPNAME = cam2map

cam2mapTruth.cub.TOLERANCE = .000001
cam2mapCheckRange.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/test.cub \
	  to=$(OUTPUT)/cam2mapTruth.cub \
	  map=$(INPUT)/test.map \
	  lonseam=continue \
	  pixres=map > /dev/null;
