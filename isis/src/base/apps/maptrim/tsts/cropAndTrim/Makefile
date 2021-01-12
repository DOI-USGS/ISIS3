APPNAME = maptrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.map.cub \
	to=$(OUTPUT)/cropAndTrimTruth.cub \
	mode=both \
	minlat=15 maxlat=20 minlon=222 maxlon=230> /dev/null;
