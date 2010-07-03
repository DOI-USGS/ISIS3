APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/f217s25.cub \
	TO=$(OUTPUT)/caminfo.pvl ISISLABEL=true ORIGINAL=true \
	STATISTICS=true CAMSTATS=true linc=25 sinc=25 \
	POLYGON=true pixinc=100 > /dev/null;
