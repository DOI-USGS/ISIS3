APPNAME = cisscal

N1536363784_1.truth.cub.TOLERANCE = 1.0e-5

include $(ISISROOT)/make/isismake.tsts


commands:
	$(APPNAME) from=$(INPUT)/N1536363784_1.c2i.spice.cub \
	to=$(OUTPUT)/N1536363784_1.truth.cub \
	flux="I/F" \
	> /dev/null;
	catlab from=$(OUTPUT)/N1536363784_1.truth.cub >& $(OUTPUT)/labels.truth.pvl;
