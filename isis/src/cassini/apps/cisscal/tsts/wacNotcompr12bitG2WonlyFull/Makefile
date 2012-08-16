APPNAME = cisscal

include $(ISISROOT)/make/isismake.tsts


commands:
	$(APPNAME) from=$(INPUT)/W1294561261_1.c2i.nospice.cub \
	to=$(OUTPUT)/W1294561261_1.truth.cub \
	> /dev/null;
	catlab from=$(OUTPUT)/W1294561261_1.truth.cub >& $(OUTPUT)/labels.truth.pvl;
