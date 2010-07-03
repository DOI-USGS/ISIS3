APPNAME = grid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	to=$(OUTPUT)/groundTruth1.cub \
	mode=ground > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	to=$(OUTPUT)/groundTruth2.cub \
	mode=ground ticks=true > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	to=$(OUTPUT)/groundTruth3.cub \
	mode=ground ticks=true diagonal=true > /dev/null;
