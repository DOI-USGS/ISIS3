APPNAME = enlarge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/enlargeTruth.cub \
	  sscale=1.0 \
	  lscale=1.5 \
	  interp=nearestneighbor > /dev/null;
