APPNAME = vims2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/fileList.txt;
	$(APPNAME) fromlist= $(OUTPUT)/fileList.txt \
	to= $(OUTPUT)/vims2map-mpp-truth.cub \
	pixres=mpp resolution=100 \
	defaultrange=camera > /dev/null;
	$(RM) $(OUTPUT)/fileList.txt;
