APPNAME = hicalproc

truth.cub.TOLERANCE = 0.000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ATL_20050526T055627_BG12_1.cub \
	to=$(OUTPUT)/truth.cub remove=true \
	destripe=true > /dev/null;
	$(RM) *.log  > /dev/null;
