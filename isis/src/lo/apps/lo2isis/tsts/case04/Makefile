APPNAME = lo2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/3083_med_isis2.cub \
	to=$(OUTPUT)/3083_med.truth.cub > /dev/null;
	catlab from=$(OUTPUT)/3083_med.truth.cub > $(OUTPUT)/labels.pvl;
