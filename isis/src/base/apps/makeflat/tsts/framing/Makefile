APPNAME = makeflat

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/fromList.lis \
	  to=$(OUTPUT)/framingFlatTruth.cub STDEV=190 \
	  IMAGETYPE=FRAMING EXCLUDE=$(OUTPUT)/framingFlatTruth.txt> /dev/null;
