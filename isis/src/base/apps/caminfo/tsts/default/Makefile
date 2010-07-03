APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/input.cub \
	TO=$(OUTPUT)/caminfo.pvl ISISLABEL=true ORIGINAL=true \
	STATISTICS=true CAMSTATS=true POLYGON=true pixinc=100 > /dev/null;
