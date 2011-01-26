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
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	to=$(OUTPUT)/groundTruth4.cub \
	mode=ground minlat=16.5 maxlat=32.5 minlon=220.5 maxlon=233.5 \
	> /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	to=$(OUTPUT)/groundTruth5.cub \
	mode=ground minlat=16.5 maxlat=32.5 minlon=220.5 maxlon=233.5 \
	boundary=true > /dev/null;
