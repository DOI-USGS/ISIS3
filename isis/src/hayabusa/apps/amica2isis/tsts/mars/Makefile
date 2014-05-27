APPNAME = amica2isis

#NOPREFERENCES=
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/st_0515854093_p.lbl \
	  to=$(OUTPUT)/st_0515854093_p.cub > /dev/null
	catlab from=$(OUTPUT)/st_0515854093_p.cub to=$(OUTPUT)/st_0515854093_p.pvl > /dev/null;
