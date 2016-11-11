APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/C1621694254_1.ir.band248.cub \
               to=$(OUTPUT)/C1621694254_1.ir.equi.cub \
			   lonseam=continue \
			   map=$(ISIS3DATA)/base/templates/maps/equirectangular.map \
			   > /dev/null;
