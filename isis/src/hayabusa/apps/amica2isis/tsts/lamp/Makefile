APPNAME = amica2isis

#NOPREFERENCES=
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/st_0015411418_w.lbl \
	  to=$(OUTPUT)/st_0015411418_w.cub > /dev/null
	catlab from=$(OUTPUT)/st_0015411418_w.cub to=$(OUTPUT)/st_0015411418_w.pvl > /dev/null;
