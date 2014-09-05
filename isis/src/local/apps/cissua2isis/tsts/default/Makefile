APPNAME = cissua2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/N1509133834_1.IMG.cal.lev1.cub \
	to=$(OUTPUT)/N1509133834_1.IMG.cal.lev1.cub > /dev/null;
	catlab from=$(OUTPUT)/N1509133834_1.IMG.cal.lev1.cub \
	to=$(OUTPUT)/N1509133834_1.IMG.cal.lev1.pvl > /dev/null;
	$(APPNAME) from=$(INPUT)/N1509135639_1.IMG.cal.lev1.cub \
	to=$(OUTPUT)/N1509135639_1.IMG.cal.lev1.cub > /dev/null;
	catlab from=$(OUTPUT)/N1509135639_1.IMG.cal.lev1.cub \
	to=$(OUTPUT)/N1509135639_1.IMG.cal.lev1.pvl > /dev/null;
