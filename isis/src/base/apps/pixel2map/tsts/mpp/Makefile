APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(APPNAME) fromtype=fromlist fromlist= $(OUTPUT)/files.lis \
	to= $(OUTPUT)/vims2map-mpp-truth.cub \
	pixres=mpp resolution=100 \
	defaultrange=camera > /dev/null;
	$(RM) $(OUTPUT)/files.lis;
