APPNAME = hicalproc

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ATL_20050526T063349_RED0_1.cub \
	to=$(OUTPUT)/truth.cub remove=true \
	furrows=true > /dev/null;
	$(RM) *.log  > /dev/null;
