APPNAME = mdiscal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/EN0001427180M.cub \
	TO=$(OUTPUT)/nonZeroPxlBin.cub \
	> /dev/null;

	catlab FROM=$(OUTPUT)/nonZeroPxlBin.cub \
	TO=$(OUTPUT)/nonZeroPxlBin.pvl \
	> /dev/null; 
