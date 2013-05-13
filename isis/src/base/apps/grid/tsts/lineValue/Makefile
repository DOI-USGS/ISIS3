APPNAME = grid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth.cub latinc=1.0 loninc=1.0 \
	  mode=ground minlat=16.5 maxlat=32.5 minlon=220.5 maxlon=233.5 \
	  boundary=true linevalue=dn dnvalue=250 > /dev/null;
	$(APPNAME) from=$(INPUT)/peaks.cub to=$(OUTPUT)/imageTruth.cub \
	  mode=image linevalue=dn dnvalue=250 > /dev/null;
	> /dev/null;
