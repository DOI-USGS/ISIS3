APPNAME = rgb2hsv

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) red=$(INPUT)/isisTruth.cub+5 \
	green=$(INPUT)/isisTruth.cub+4 \
	blue=$(INPUT)/isisTruth.cub+3 \
	hue=$(OUTPUT)/rgb2hsvTruth1.cub \
	saturation=$(OUTPUT)/rgb2hsvTruth2.cub \
	value=$(OUTPUT)/rgb2hsvTruth3.cub \
	mode=automatic > /dev/null;
