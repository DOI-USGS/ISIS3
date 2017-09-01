APPNAME = cisscal

N1536363784_1.truth.cub.TOLERANCE = 1.0e-5

include $(ISISROOT)/make/isismake.tsts


commands:
	$(APPNAME) from=$(INPUT)/N1523990693_1.cub \
	to=$(OUTPUT)/N1523990693_1.truth.cub \
	> /dev/null;
	catlab from=$(OUTPUT)/N1523990693_1.truth.cub to=$(OUTPUT)/labels.truth.pvl > /dev/null;
