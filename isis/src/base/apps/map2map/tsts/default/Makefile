APPNAME = map2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	  map=$(INPUT)/ab102402.lev2.cub \
	  to=$(OUTPUT)/map2mapTruth.cub \
          defaultrange=map INTERP=NEARESTNEIGHBOR > /dev/null;
