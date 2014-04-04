APPNAME = chan1m32isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/M3G20090106T113423_V03_L1B.LBL \
	 loc=$(OUTPUT)/loc.cub \
	 obs=$(OUTPUT)/obs.cub \
         to=$(OUTPUT)/M3G20090106T113423_V03_RDN.cub > /dev/null;
	catlab from=$(OUTPUT)/M3G20090106T113423_V03_RDN.cub  \
	  > $(OUTPUT)/labels.pvl;
