APPNAME = marci2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/MOI_000009_0294_MU_00N044W.IMG TO=$(OUTPUT)/out.cub > /dev/null;
	catlab FROM=$(OUTPUT)/out.even.cub to=$(OUTPUT)/even.pvl > /dev/null;
	catlab FROM=$(OUTPUT)/out.odd.cub to=$(OUTPUT)/odd.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/T02_001251_1292_MU_00N237W.IMG flip=NO TO=$(OUTPUT)/unflipped.cub > /dev/null;
	catlab FROM=$(OUTPUT)/unflipped.even.cub to=$(OUTPUT)/unflipped.pvl > /dev/null;
