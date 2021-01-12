APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitdefault.img \
	bittype=s16bit > /dev/null;
	catlab from=$(OUTPUT)/s16bitdefault.img to=$(OUTPUT)/s16bitdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitdefault.img to=$(OUTPUT)/s16bitdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitnonull.img \
	bittype=s16bit null=no > /dev/null;
	catlab from=$(OUTPUT)/s16bitnonull.img to=$(OUTPUT)/s16bitnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitnonull.img to=$(OUTPUT)/s16bitnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitspecialpixels.img \
	bittype=s16bit lrs=yes lis=yes his=yes hrs=yes > /dev/null;
	catlab from=$(OUTPUT)/s16bitspecialpixels.img to=$(OUTPUT)/s16bitspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitspecialpixels.img to=$(OUTPUT)/s16bitspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitspecialpixels.img > /dev/null;
