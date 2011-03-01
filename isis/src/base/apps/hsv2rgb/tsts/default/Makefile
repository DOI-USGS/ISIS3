APPNAME = hsv2rgb

redTruth.cub.TOLERANCE = 0.0001
blueTruth.cub.TOLERANCE = 0.0001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) hue=$(INPUT)/hueTruth.cub \
	saturation=$(INPUT)/satTruth.cub \
	value=$(INPUT)/valTruth.cub \
	red=$(OUTPUT)/redTruth.cub \
	green=$(OUTPUT)/greenTruth.cub \
	blue=$(OUTPUT)/blueTruth.cub > /dev/null;
