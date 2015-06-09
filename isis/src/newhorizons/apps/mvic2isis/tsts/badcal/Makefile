APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# Tests for when a calibrated file does not have some of the expected keys
	LC_CTYPE=C && LANG=C && $(SED) \
	  -e 's+PIXSIZE+PIX-IZE+' \
	  -e 's+PIXFOV+PIX-OV+' \
	  -e 's+GAIN+GA-N+' \
	  -e 's+READNOI+RE-DNOI+' \
	  -e 's+TDIRATE+TDIR-TE+' \
	  $(INPUT)/mp1_0035135818_0x530_sci_1.fit > $(OUTPUT)/temp.fit;
	$(APPNAME) from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/mp1_0035135818_0x530_sci_1_Truth.cub > /dev/null;
	catlab from=$(OUTPUT)/mp1_0035135818_0x530_sci_1_Truth.cub > \
	  $(OUTPUT)/mp1_0035135818_0x530_sci_1_Truth.pvl;
	$(RM) $(OUTPUT)/temp.fit;




