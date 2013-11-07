APPNAME = cisscal

N1536363784_1.truth.cub.TOLERANCE = 1.0e-5

include $(ISISROOT)/make/isismake.tsts


commands:
	$(APPNAME) from=$(INPUT)/N1597182735_2.cub \
	to=$(OUTPUT)/N1597182735_2.truth.cub \
	> /dev/null;
	catlab from=$(OUTPUT)/N1597182735_2.truth.cub >& $(OUTPUT)/labels.truth.pvl;
