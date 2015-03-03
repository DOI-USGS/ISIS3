APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/ground.lis \
	to=$(OUTPUT)/pointlistGroundTruth.pvl \
	append=false coordtype=GROUND > /dev/null;
