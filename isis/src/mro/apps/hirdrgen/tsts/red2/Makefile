APPNAME = hirdrgen

test_himos_070418_8_reimport.cub.TOLERANCE = .00026

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test_himos_070418.cub \
	           to=$(OUTPUT)/test_himos_070418_8.img bits=8 minper=0.5 maxper=99.5> /dev/null;
	catlab from=$(OUTPUT)/test_himos_070418_8.img \
		> $(OUTPUT)/test_himos_070418_8_imglabels.pvl;
	pds2isis from=$(OUTPUT)/test_himos_070418_8.img \
	         to=$(OUTPUT)/test_himos_070418_8_reimport.cub > /dev/null;
	rm $(OUTPUT)/test_himos_070418_8.img;

