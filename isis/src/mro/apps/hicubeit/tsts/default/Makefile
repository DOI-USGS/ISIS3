APPNAME = hicubeit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) RED=$(INPUT)/RED4slither.cub \
	IR=$(INPUT)/IR10slither.cub BG=$(INPUT)/BG12slither.cub \
	TO=$(OUTPUT)/slither.cub  > /dev/null;
	catlab FROM=$(OUTPUT)/slither.cub >& $(OUTPUT)/slither.pvl;
