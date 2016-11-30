APPNAME = hirdr2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/test_himos_070418.img \
	TO=$(OUTPUT)/test_himos_070418.cub > /dev/null;
	catlab from=$(OUTPUT)/test_himos_070418.cub \
				to=$(OUTPUT)/test_himos_070418.pvl > /dev/null;
   
