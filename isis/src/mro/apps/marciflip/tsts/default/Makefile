APPNAME = marciflip

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/T02_001002_1200_MC_00N284W.cub \
	TO=$(OUTPUT)/flipped.cub > /dev/null;
	catlab FROM=$(OUTPUT)/flipped.cub > $(OUTPUT)/flipped.pvl;
