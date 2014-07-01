APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) from=$(INPUT)/nac100015d79_proj_no_units.cub \
	to=$(OUTPUT)/nac100015d79_proj_no_units.img  > /dev/null;
	catlab from=$(OUTPUT)/nac100015d79_proj_no_units.img \
        to=$(OUTPUT)/nac100015d79_proj_no_units.pvl > /dev/null; 
	$(APPNAME) from=$(INPUT)/nac100015d79_proj.cub \
	to=$(OUTPUT)/nac100015d79_proj.img  > /dev/null;
	catlab from=$(OUTPUT)/nac100015d79_proj.img \
        to=$(OUTPUT)/nac100015d79_proj.pvl > /dev/null; 



