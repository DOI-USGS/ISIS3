APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitmanualdefault.img \
	bittype=8bit stretch=manual minimum=0 maximum=255 > /dev/null;
	catlab from=$(OUTPUT)/8bitmanualdefault.img to=$(OUTPUT)/8bitmanualdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitmanualdefault.img to=$(OUTPUT)/8bitmanualdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitmanualdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitmanualnonull.img \
	bittype=8bit null=no stretch=manual minimum=0 maximum=255 > /dev/null;
	catlab from=$(OUTPUT)/8bitmanualnonull.img to=$(OUTPUT)/8bitmanualnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitmanualnonull.img to=$(OUTPUT)/8bitmanualnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitmanualnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitmanualspecialpixels.img \
	bittype=8bit lrs=yes lis=yes his=yes hrs=yes stretch=manual minimum=0 maximum=255 > /dev/null;
	catlab from=$(OUTPUT)/8bitmanualspecialpixels.img to=$(OUTPUT)/8bitmanualspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitmanualspecialpixels.img to=$(OUTPUT)/8bitmanualspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitmanualspecialpixels.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitmanualdefault.img \
	bittype=s16bit stretch=manual minimum=-32768 maximum=32767 > /dev/null;
	catlab from=$(OUTPUT)/s16bitmanualdefault.img to=$(OUTPUT)/s16bitmanualdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitmanualdefault.img to=$(OUTPUT)/s16bitmanualdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitmanualdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitmanualnonull.img \
	bittype=s16bit null=no stretch=manual minimum=-32768 maximum=32767 > /dev/null;
	catlab from=$(OUTPUT)/s16bitmanualnonull.img to=$(OUTPUT)/s16bitmanualnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitmanualnonull.img to=$(OUTPUT)/s16bitmanualnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitmanualnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitmanualspecialpixels.img \
	bittype=s16bit lrs=yes lis=yes his=yes hrs=yes stretch=manual minimum=-32768 maximum=32767 > /dev/null;
	catlab from=$(OUTPUT)/s16bitmanualspecialpixels.img to=$(OUTPUT)/s16bitmanualspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitmanualspecialpixels.img to=$(OUTPUT)/s16bitmanualspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitmanualspecialpixels.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitmanualdefault.img \
	bittype=u16bit stretch=manual minimum=0 maximum=65535 > /dev/null;
	catlab from=$(OUTPUT)/u16bitmanualdefault.img to=$(OUTPUT)/u16bitmanualdefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitmanualdefault.img to=$(OUTPUT)/u16bitmanualdefault.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitmanualdefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitmanualnonull.img \
	bittype=u16bit null=no stretch=manual minimum=0 maximum=65535 > /dev/null;
	catlab from=$(OUTPUT)/u16bitmanualnonull.img to=$(OUTPUT)/u16bitmanualnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitmanualnonull.img to=$(OUTPUT)/u16bitmanualnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitmanualnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitmanualspecialpixels.img \
	bittype=u16bit lrs=yes lis=yes his=yes hrs=yes stretch=manual minimum=0 maximum=65535 > /dev/null;
	catlab from=$(OUTPUT)/u16bitmanualspecialpixels.img to=$(OUTPUT)/u16bitmanualspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitmanualspecialpixels.img to=$(OUTPUT)/u16bitmanualspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitmanualspecialpixels.img > /dev/null;
