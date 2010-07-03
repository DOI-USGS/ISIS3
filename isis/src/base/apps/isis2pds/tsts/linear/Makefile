APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitlineardefault.img \
	bittype=8bit stretch=linear > /dev/null;
	catlab from=$(OUTPUT)/8bitlineardefault.img to=$(OUTPUT)/8bitlineardefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitlineardefault.img to=$(OUTPUT)/8bitlineardefault.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitlineardefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitlinearnonull.img \
	bittype=8bit null=no stretch=linear minpercent=5.0 maxpercent=45.0 > /dev/null;
	catlab from=$(OUTPUT)/8bitlinearnonull.img to=$(OUTPUT)/8bitlinearnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitlinearnonull.img to=$(OUTPUT)/8bitlinearnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitlinearnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/8bittruth.cub to=$(OUTPUT)/8bitlinearspecialpixels.img \
	bittype=8bit lrs=yes lis=yes his=yes hrs=yes stretch=linear minpercent=45.5 maxpercent=65.2 > /dev/null;
	catlab from=$(OUTPUT)/8bitlinearspecialpixels.img to=$(OUTPUT)/8bitlinearspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/8bitlinearspecialpixels.img to=$(OUTPUT)/8bitlinearspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/8bitlinearspecialpixels.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitlineardefault.img \
	bittype=s16bit stretch=linear > /dev/null;
	catlab from=$(OUTPUT)/s16bitlineardefault.img to=$(OUTPUT)/s16bitlineardefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitlineardefault.img to=$(OUTPUT)/s16bitlineardefault.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitlineardefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitlinearnonull.img \
	bittype=s16bit null=no stretch=linear minpercent=2.0 maxpercent=5.0 > /dev/null;
	catlab from=$(OUTPUT)/s16bitlinearnonull.img to=$(OUTPUT)/s16bitlinearnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitlinearnonull.img to=$(OUTPUT)/s16bitlinearnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitlinearnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/s16bitlinearspecialpixels.img \
	bittype=s16bit lrs=yes lis=yes his=yes hrs=yes stretch=linear minpercent=45.0 maxpercent=55.0 > /dev/null;
	catlab from=$(OUTPUT)/s16bitlinearspecialpixels.img to=$(OUTPUT)/s16bitlinearspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/s16bitlinearspecialpixels.img to=$(OUTPUT)/s16bitlinearspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/s16bitlinearspecialpixels.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitlineardefault.img \
	bittype=u16bit stretch=linear > /dev/null;
	catlab from=$(OUTPUT)/u16bitlineardefault.img to=$(OUTPUT)/u16bitlineardefaultlabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitlineardefault.img to=$(OUTPUT)/u16bitlineardefault.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitlineardefault.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitlinearnonull.img \
	bittype=u16bit null=no stretch=linear minpercent=90.0 maxpercent=95.0 > /dev/null;
	catlab from=$(OUTPUT)/u16bitlinearnonull.img to=$(OUTPUT)/u16bitlinearnonulllabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitlinearnonull.img to=$(OUTPUT)/u16bitlinearnonull.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitlinearnonull.img > /dev/null;
	$(APPNAME) from=$(INPUT)/s16bittruth.cub to=$(OUTPUT)/u16bitlinearspecialpixels.img \
	bittype=u16bit lrs=yes lis=yes his=yes hrs=yes stretch=linear minpercent=1.0 maxpercent=99.0 > /dev/null;
	catlab from=$(OUTPUT)/u16bitlinearspecialpixels.img to=$(OUTPUT)/u16bitlinearspecialpixelslabels.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/u16bitlinearspecialpixels.img to=$(OUTPUT)/u16bitlinearspecialpixels.cub > /dev/null;
	$(RM) $(OUTPUT)/u16bitlinearspecialpixels.img > /dev/null;
