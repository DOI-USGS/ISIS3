APPNAME = cisscal

include $(ISISROOT)/make/isismake.tsts


commands:
	$(APPNAME) from=$(INPUT)/N1355543510_1.c2i.nospice.cub \
	to=$(OUTPUT)/N1355543510_1.truth.cub \
	units="INTENSITY" \
	> /dev/null;
	catlab from=$(OUTPUT)/N1355543510_1.truth.cub to=$(OUTPUT)/labels.truth.pvl > /dev/null;
