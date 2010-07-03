APPNAME = hsv2rgb

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) hue=$(INPUT)/hueTruth.cub \
	saturation=$(INPUT)/satTruth.cub \
	value=$(INPUT)/valTruth.cub \
	red=$(OUTPUT)/redTruth.cub \
	green=$(OUTPUT)/greenTruth.cub \
	blue=$(OUTPUT)/blueTruth.cub > /dev/null;
