APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitmsbdefault.img \
	bittype=s16bit endian=msb > /dev/null;
	catlab from=$(OUTPUT)/s16bitmsbdefault.img to=$(OUTPUT)/s16bitmsbdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitmsbdefault.img to=$(OUTPUT)/s16bitmsbdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitmsbdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitmsbnonull.img \
	bittype=s16bit null=no endian=msb > /dev/null;
	catlab from=$(OUTPUT)/s16bitmsbnonull.img to=$(OUTPUT)/s16bitmsbnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitmsbnonull.img to=$(OUTPUT)/s16bitmsbnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitmsbnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitmsbspecialpixels.img \
	bittype=s16bit lrs=yes lis=yes his=yes hrs=yes endian=msb > /dev/null;
	catlab from=$(OUTPUT)/s16bitmsbspecialpixels.img to=$(OUTPUT)/s16bitmsbspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitmsbspecialpixels.img to=$(OUTPUT)/s16bitmsbspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitmsbspecialpixels.img > /dev/null;
