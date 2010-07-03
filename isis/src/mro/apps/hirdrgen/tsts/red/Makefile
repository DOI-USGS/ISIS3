APPNAME = hirdrgen

test_himos_070418_reimport.cub.TOLERANCE = .00019
test_himos_070418_16_reimport.cub.TOLERANCE = .00025
test_himos_polar_070508_reimport.cub.TOLERANCE = .00026

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test_himos_070418.cub \
	           to=$(OUTPUT)/test_himos_070418.img > /dev/null;
	catlab from=$(OUTPUT)/test_himos_070418.img \
		> $(OUTPUT)/test_himos_070418_imglabels.pvl;
	pds2isis from=$(OUTPUT)/test_himos_070418.img \
	         to=$(OUTPUT)/test_himos_070418_reimport.cub > /dev/null;
	rm $(OUTPUT)/test_himos_070418.img;

	$(APPNAME) from=$(INPUT)/test_himos_070418.cub \
	           to=$(OUTPUT)/test_himos_070418_16.img bits=16 > /dev/null;
	catlab from=$(OUTPUT)/test_himos_070418_16.img \
		> $(OUTPUT)/test_himos_070418_16_imglabels.pvl;
	pds2isis from=$(OUTPUT)/test_himos_070418_16.img \
	         to=$(OUTPUT)/test_himos_070418_16_reimport.cub > /dev/null;
	rm $(OUTPUT)/test_himos_070418_16.img;

	$(APPNAME) from=$(INPUT)/test_himos_polar_070508.cub \
	           to=$(OUTPUT)/test_himos_polar_070508.img bits=8 minper=0.5 maxper=99.5> /dev/null;
	catlab from=$(OUTPUT)/test_himos_polar_070508.img \
		> $(OUTPUT)/test_himos_polar_070508_imglabels.pvl;
	pds2isis from=$(OUTPUT)/test_himos_polar_070508.img \
	         to=$(OUTPUT)/test_himos_polar_070508_reimport.cub > /dev/null;
	rm $(OUTPUT)/test_himos_polar_070508.img;

