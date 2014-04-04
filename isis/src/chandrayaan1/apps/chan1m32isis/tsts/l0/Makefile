APPNAME = chan1m32isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/M3G20090106T113423_V01_L0.LBL \
         to=$(OUTPUT)/M3G20090106T113423_V01_L0.cub > /dev/null;
	catlab from=$(OUTPUT)/M3G20090106T113423_V01_L0.cub  \
	 to=$(OUTPUT)/M3G20090106T113423_V01_L0.pvl >& /dev/null;
