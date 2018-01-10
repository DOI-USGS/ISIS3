# Updated by Kristin Berry 2017-12-21 for new test data and significant updates to tgocassisi2isis

APPNAME = tgocassis2isis

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.xml \
	           to=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub > /dev/null;

	catlab from=$(OUTPUT)/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub \
	       to=$(OUTPUT)/labels.pvl > /dev/null;
