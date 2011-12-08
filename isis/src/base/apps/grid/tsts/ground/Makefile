APPNAME = grid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth1.cub latinc=1.0 loninc=1.0 \
	  mode=ground > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth2.cub latinc=1.0 loninc=1.0 \
	  mode=ground ticks=true > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth3.cub latinc=1.0 loninc=1.0 \
	  mode=ground ticks=true diagonal=true > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth4.cub latinc=1.0 loninc=1.0 \
	  mode=ground minlat=16.5 maxlat=32.5 minlon=220.5 maxlon=233.5 \
	  > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth5.cub latinc=1.0 loninc=1.0 \
	  mode=ground minlat=16.5 maxlat=32.5 minlon=220.5 maxlon=233.5 \
	  boundary=true > /dev/null;
	$(APPNAME) from=$(INPUT)/messenger_reduced.cub \
	  to=$(OUTPUT)/groundTruth6.cub outline=true baselat=-70 > /dev/null
