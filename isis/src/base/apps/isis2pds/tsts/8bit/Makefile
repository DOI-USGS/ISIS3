APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitdefault.img \
	bittype=8bit > /dev/null;
	catlab from=$(OUTPUT)/8bitdefault.img to=$(OUTPUT)/8bitdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitdefault.img to=$(OUTPUT)/8bitdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitnonull.img \
	bittype=8bit null=no > /dev/null;
	catlab from=$(OUTPUT)/8bitnonull.img to=$(OUTPUT)/8bitnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitnonull.img to=$(OUTPUT)/8bitnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitspecialpixels.img \
	bittype=8bit lrs=yes lis=yes his=yes hrs=yes > /dev/null;
	catlab from=$(OUTPUT)/8bitspecialpixels.img to=$(OUTPUT)/8bitspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitspecialpixels.img to=$(OUTPUT)/8bitspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitspecialpixels.img > /dev/null;
