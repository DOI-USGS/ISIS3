APPNAME = gllssical

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/0100r.cub TO=$(OUTPUT)/0100r.cal.cub > /dev/null;
	catlab FROM=$(OUTPUT)/0100r.cal.cub TO=$(OUTPUT)/0100r.cal.pvl > /dev/null;
