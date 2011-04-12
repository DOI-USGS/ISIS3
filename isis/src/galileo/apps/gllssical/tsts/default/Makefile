APPNAME = gllssical

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/3439R.cub TO=$(OUTPUT)/3439R.cal.cub > /dev/null;
	catlab FROM=$(OUTPUT)/3439R.cal.cub TO=$(OUTPUT)/3439R.cal.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/1213r.cub TO=$(OUTPUT)/1213r.cal.cub > /dev/null;
	catlab FROM=$(OUTPUT)/1213r.cal.cub TO=$(OUTPUT)/1213r.cal.pvl > /dev/null;
