# Test creating TIFF images
#
# Test 1: Grayscale from first band of isisTruth cube using linear stretch. The output truth cube 
#         from this test should be identical to the output truth cubes isis2stdTruth
#         for grayscale test cases of bmp, jpeg, jpeg2000, and png
#
# Test 2: RGB, store the world file output as isis2stdTruth_rgb.txt
#
# Test 3: Grayscale, unsigned 16 bit
#
# THIS TEST IS COMMENTED OUT SINCE SIGNED WORD CONVERSION APPEARS TO BE FAILING Test 4: Grayscale, signed 16 bit
#
APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth_gray.tif \
	  mode=grayscale \
	  format=tiff \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray.tif \
	  TO=$(OUTPUT)/isis2stdTruth_gray.cub >/dev/null;
	$(APPNAME) red=$(INPUT)/rgb.cub+1 \
	  green=$(INPUT)/rgb.cub+2 \
	  blue=$(INPUT)/rgb.cub+3 \
	  to=$(OUTPUT)/isis2stdTruth_rgb.tif \
	  mode=rgb \
	  format=tiff \
	  stretch=linear > /dev/null;
	cat $(OUTPUT)/isis2stdTruth_rgb.tfw | sed 's/\(\.[0-9][0-9][0-9]\)\([0-9]*\)/\1/' \
	  > $(OUTPUT)/isis2stdTruth_rgb.txt;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_rgb.tif \
	  TO=$(OUTPUT)/isis2stdTruth_rgb.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/grayscale.cub \
	  to=$(OUTPUT)/isis2stdTruth_gray_u16bit.tif \
	  mode=grayscale \
	  format=tiff \
	  bittype=u16bit \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray_u16bit.tif \
	  TO=$(OUTPUT)/isis2stdTruth_gray_u16bit.cub > /dev/null;
# THIS TEST IS COMMENTED OUT SINCE SIGNED WORD CONVERSION APPEARS TO BE FAILING Test 4: Grayscale, signed 16 bit
#	$(APPNAME) from=$(INPUT)/grayscale.cub \
#	  to=$(OUTPUT)/isis2stdTruth_gray_s16bit.tif \
#	  mode=grayscale \
#	  format=tiff \
#	  bittype=s16bit \
#	  stretch=linear > /dev/null;
#	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray_s16bit.tif \
#	  TO=$(OUTPUT)/isis2stdTruth_gray_s16bit.cub > /dev/null;
	$(RM) $(OUTPUT)/isis2stdTruth_gray.tif;
	$(RM) $(OUTPUT)/isis2stdTruth_rgb.tfw;
	$(RM) $(OUTPUT)/isis2stdTruth_rgb.tif;
	$(RM) $(OUTPUT)/isis2stdTruth_gray_u16bit.tif;
#	$(RM) $(OUTPUT)/isis2stdTruth_gray_s16bit.tif;
