APPNAME = pvldiff

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/pvl1.pvl FROM2=$(INPUT)/pvl2.pvl \
	DIFF=$(INPUT)/tolerance.pvl TO=$(OUTPUT)/diff.txt > /dev/null;
