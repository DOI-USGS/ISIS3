APPNAME = makeflat

linescanFlatTruth.cub.TOLERANCE = 0.0000002

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/fromList.lis \
	  to=$(OUTPUT)/linescanFlatTruth.cub STDEV=190 \
	  IMAGETYPE=LINESCAN NUMLINES=10 \
	  EXCLUDE=$(OUTPUT)/linescanFlatTruth.txt > /dev/null;
