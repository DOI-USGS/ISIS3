APPNAME = amica2isis

#NOPREFERENCES=
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/st_2458542208_v.lbl \
	  to=$(OUTPUT)/st_2458542208_v.cub > /dev/null;
	catlab from=$(OUTPUT)/st_2458542208_v.cub to=$(OUTPUT)/st_2458542208_v.pvl > /dev/null;
