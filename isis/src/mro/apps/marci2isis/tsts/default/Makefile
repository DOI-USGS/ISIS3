APPNAME = marci2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/MOI_000009_0294_MU_00N044W.IMG TO=$(OUTPUT)/out.cub > /dev/null;
	catlab FROM=$(OUTPUT)/out.even.cub > $(OUTPUT)/even.pvl;
	catlab FROM=$(OUTPUT)/out.odd.cub > $(OUTPUT)/odd.pvl;
	$(APPNAME) FROM=$(INPUT)/T02_001251_1292_MU_00N237W.IMG flip=NO TO=$(OUTPUT)/unflipped.cub > /dev/null;
	catlab FROM=$(OUTPUT)/unflipped.even.cub > $(OUTPUT)/unflipped.pvl;
