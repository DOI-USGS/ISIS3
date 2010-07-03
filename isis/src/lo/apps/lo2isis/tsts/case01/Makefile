APPNAME = lo2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/3133_h1.cub \
	to=$(OUTPUT)/3133_h1.truth.cub > /dev/null;
	catlab from=$(OUTPUT)/3133_h1.truth.cub >& $(OUTPUT)/labels.pvl;
