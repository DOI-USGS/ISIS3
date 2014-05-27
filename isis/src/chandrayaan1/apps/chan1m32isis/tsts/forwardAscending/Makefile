APPNAME = chan1m32isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/M3T20090630T083407_V03_L1B.LBL \
	 loc=$(OUTPUT)/loc.cub \
	 obs=$(OUTPUT)/obs.cub \
         to=$(OUTPUT)/M3T20090630T083407_V03_RDN.cub > /dev/null;
	catlab from=$(OUTPUT)/M3T20090630T083407_V03_RDN.cub  \
	  > $(OUTPUT)/labels.pvl;
