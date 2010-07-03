APPNAME = specpix

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/specpixTruth3.cub \
	  hrsmin=1 \
	  hrsmax=100 > /dev/null;
