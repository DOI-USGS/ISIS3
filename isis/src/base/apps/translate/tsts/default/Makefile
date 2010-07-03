APPNAME = translate

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/translateTruth1.cub \
	  strans=10.5 \
	  ltrans=-10.5 \
	  interp=nearestneighbor > /dev/null;
