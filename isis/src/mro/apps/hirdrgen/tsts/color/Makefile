APPNAME = hirdrgen

include $(ISISROOT)/make/isismake.tsts


commands:
	$(APPNAME) from=$(INPUT)/test_color_070419.cub \
	           to=$(OUTPUT)/test_color_070419.img version=3.0> /dev/null;
	catlab from=$(OUTPUT)/test_color_070419.img \
		> $(OUTPUT)/test_color_070419_imglabels.pvl;
	pds2isis from=$(OUTPUT)/test_color_070419.img \
	         to=$(OUTPUT)/test_color_070419_reimport.cub > /dev/null;
	rm $(OUTPUT)/test_color_070419.img;
