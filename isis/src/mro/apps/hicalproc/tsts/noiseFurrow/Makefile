APPNAME = hicalproc

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ATL_20050526T063349_IR10_0.cub \
	to=$(OUTPUT)/truth.cub remove=true \
	furrows=true noise_filter=true > /dev/null;
	$(RM) *.log  > /dev/null;
