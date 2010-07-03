APPNAME = specpix

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/specpixTruth5.cub \
	  hismin=1 \
	  hismax=100 > /dev/null;
