APPNAME = gllssical

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/3800r.cub TO=$(OUTPUT)/3800r.cal.cub UNITS=RADIANCE > /dev/null;
	catlab FROM=$(OUTPUT)/3800r.cal.cub TO=$(OUTPUT)/3800r.cal.pvl > /dev/null;
