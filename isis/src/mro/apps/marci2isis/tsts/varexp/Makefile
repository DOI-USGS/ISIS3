APPNAME = marci2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/P07_003640_2331_MA_00N288W.IMG TO=$(OUTPUT)/out.cub > /dev/null;
	catlab FROM=$(OUTPUT)/out.even.cub to=$(OUTPUT)/even.pvl > /dev/null;
	catlab FROM=$(OUTPUT)/out.odd.cub to=$(OUTPUT)/odd.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/P07_003640_2331_MA_00N288W.IMG flip=NO TO=$(OUTPUT)/unflipped.cub > /dev/null;
	catlab FROM=$(OUTPUT)/unflipped.even.cub to=$(OUTPUT)/unflipped.pvl > /dev/null;
