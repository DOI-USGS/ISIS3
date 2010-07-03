APPNAME = pvldiff

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/pvl1.pvl FROM2=$(INPUT)/pvl2.pvl \
	TO=$(OUTPUT)/diff_simp.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/pvl2.pvl FROM2=$(INPUT)/pvl3.pvl \
	TO=$(OUTPUT)/diff_units.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/pvl4.pvl FROM2=$(INPUT)/pvl5.pvl \
	TO=$(OUTPUT)/diff_complex.pvl > /dev/null;
