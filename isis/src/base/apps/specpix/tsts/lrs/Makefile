APPNAME = specpix

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/specpixTruth2.cub \
	  lrsmin=1 \
	  lrsmax=100 > /dev/null;
