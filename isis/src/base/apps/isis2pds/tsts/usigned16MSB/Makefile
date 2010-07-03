APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitmsbdefault.img \
	bittype=u16bit endian=msb > /dev/null;
	catlab from=$(OUTPUT)/u16bitmsbdefault.img to=$(OUTPUT)/u16bitmsbdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitmsbdefault.img to=$(OUTPUT)/u16bitmsbdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitmsbdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitmsbnonull.img \
	bittype=u16bit null=no endian=msb > /dev/null;
	catlab from=$(OUTPUT)/u16bitmsbnonull.img to=$(OUTPUT)/u16bitmsbnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitmsbnonull.img to=$(OUTPUT)/u16bitmsbnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitmsbnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitmsbspecialpixels.img \
	bittype=u16bit lrs=yes lis=yes his=yes hrs=yes endian=msb > /dev/null;
	catlab from=$(OUTPUT)/u16bitmsbspecialpixels.img to=$(OUTPUT)/u16bitmsbspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitmsbspecialpixels.img to=$(OUTPUT)/u16bitmsbspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitmsbspecialpixels.img > /dev/null;
