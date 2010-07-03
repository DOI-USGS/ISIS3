APPNAME = hirdrgen

test_himos_070418_reimport.cub.TOLERANCE = .00019
test_himos_070418_16_reimport.cub.TOLERANCE = .00025

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test_himos_070418.cub \
	           to=$(OUTPUT)/test_himos_070418.img type=manual \
		   min=0.0265114 max=0.107671 version=2.0 \
	           rationale='Testing123' > /dev/null;
	catlab from=$(OUTPUT)/test_himos_070418.img \
		> $(OUTPUT)/test_himos_070418_imglabels.pvl;
	pds2isis from=$(OUTPUT)/test_himos_070418.img \
	         to=$(OUTPUT)/test_himos_070418_reimport.cub > /dev/null;
	rm $(OUTPUT)/test_himos_070418.img;

	$(APPNAME) from=$(INPUT)/test_himos_polar_070508.cub \
	           to=$(OUTPUT)/test_himos_polar_070508.img bits=8 type=manual \
		   min=0.0359667 max=0.0931104> /dev/null;
	catlab from=$(OUTPUT)/test_himos_polar_070508.img \
		> $(OUTPUT)/test_himos_polar_070508_imglabels.pvl;
	pds2isis from=$(OUTPUT)/test_himos_polar_070508.img \
	         to=$(OUTPUT)/test_himos_polar_070508_reimport.cub > /dev/null;
	rm $(OUTPUT)/test_himos_polar_070508.img;

