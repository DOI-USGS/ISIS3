APPNAME = cisscal

N1536363784_1.truth.cub.TOLERANCE = 1.0e-5

include $(ISISROOT)/make/isismake.tsts


commands:
	$(APPNAME) from=$(INPUT)/W1477592960_1.cub \
	to=$(OUTPUT)/W1477592960_1.truth.cub \
	> /dev/null;
	catlab from=$(OUTPUT)/W1477592960_1.truth.cub to=$(OUTPUT)/labels.truth.pvl > /dev/null;
