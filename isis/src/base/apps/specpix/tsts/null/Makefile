APPNAME = specpix

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/specpixTruth1.cub \
	  nullmin=1 \
	  nullmax=100 > /dev/null;
