APPNAME = chan1m32isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/M3G20081118T223204_V03_L1B.LBL \
         to=$(OUTPUT)/M3G20081118T223204_V03_RDN.cub > /dev/null;
	catlab from=$(OUTPUT)/M3G20081118T223204_V03_RDN.cub  \
	  > $(OUTPUT)/labels.pvl;
