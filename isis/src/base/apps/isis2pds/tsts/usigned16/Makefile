APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitdefault.img \
	bittype=u16bit > /dev/null;
	catlab from=$(OUTPUT)/u16bitdefault.img to=$(OUTPUT)/u16bitdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitdefault.img to=$(OUTPUT)/u16bitdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitnonull.img \
	bittype=u16bit null=no > /dev/null;
	catlab from=$(OUTPUT)/u16bitnonull.img to=$(OUTPUT)/u16bitnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitnonull.img to=$(OUTPUT)/u16bitnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitspecialpixels.img \
	bittype=u16bit lrs=yes lis=yes his=yes hrs=yes > /dev/null;
	catlab from=$(OUTPUT)/u16bitspecialpixels.img to=$(OUTPUT)/u16bitspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitspecialpixels.img to=$(OUTPUT)/u16bitspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitspecialpixels.img > /dev/null;
