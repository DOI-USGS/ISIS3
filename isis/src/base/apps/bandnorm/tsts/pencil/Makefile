APPNAME = bandnorm

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME)  from=$(INPUT)/peaks.cub \
	to=$(OUTPUT)/bandnormPeaksTruth.cub \
	AVERAGE=pencil SPECTRUM=$(INPUT)/peakspencil.txt \
	> /dev/null;
	$(APPNAME)  from=$(INPUT)/m0402852.cub \
	to=$(OUTPUT)/bandnormM040Truth.cub \
	AVERAGE=pencil SPECTRUM=$(INPUT)/m040285pencil.txt \
	> /dev/null;
	$(APPNAME)  from=$(INPUT)/I00831002RDRcropped.cub \
	to=$(OUTPUT)/bandnormI008Truth.cub \
	AVERAGE=pencil SPECTRUM=$(INPUT)/I00831002RDRpencil.txt \
	> /dev/null;
	$(APPNAME)  from=$(INPUT)/I00831002RDRcropped.cub \
	to=$(OUTPUT)/bandnormI008NumTruth.cub \
	AVERAGE=pencil SPECTRUM=$(INPUT)/I00831002RDRpencil.txt \
	METHOD=bynumber NUMBER=6 \
	> /dev/null;
