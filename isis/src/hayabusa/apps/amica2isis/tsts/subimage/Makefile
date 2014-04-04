APPNAME = amica2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/st_2489244561_v.lbl \
	  to=$(OUTPUT)/st_2489244561_v.cub > /dev/null
	catlab from=$(OUTPUT)/st_2489244561_v.cub to=$(OUTPUT)/st_2489244561_v.pvl > /dev/null;
