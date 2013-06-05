# Test JPEG2000 images
# Test 1: Grayscale from first band of isisTruth cube using linear stretch. The output truth cube 
#         from this test should be identical to the output truth cubes isis2stdTruth
#         for grayscale test cases of bmp, jpeg, png, and tiff
#
# Test 2: RGB
#
# Test 3: ARGB
#
# Test 4: Grayscale, unsigned 16 bit
#
# THIS TEST IS COMMENTED OUT SINCE SIGNED WORD CONVERSION APPEARS TO BE FAILING Test 4: Grayscale, signed 16 bit
APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth_gray.jp2 \
	  mode=grayscale \
	  format=jp2 \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray.jp2 \
	  TO=$(OUTPUT)/isis2stdTruth_gray.cub >/dev/null;
	$(APPNAME) red=$(INPUT)/PSP_001892_1715_COLOR.cub+1 \
	  green=$(INPUT)/PSP_001892_1715_COLOR.cub+2 \
	  blue=$(INPUT)/PSP_001892_1715_COLOR.cub+3 \
	  to=$(OUTPUT)/isis2stdTruth_rgb.jp2 \
	  mode=rgb \
	  format=jp2 \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_rgb.jp2 \
	  TO=$(OUTPUT)/isis2stdTruth_rgb.cub > /dev/null;
	$(APPNAME) alpha=$(INPUT)/isisTruth.cub+1 \
	  red=$(INPUT)/isisTruth.cub+1 \
	  green=$(INPUT)/isisTruth.cub+1 \
	  blue=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2stdTruth_argb.jp2 \
	  mode=argb \
	  format=jp2 \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_argb.jp2 \
	  TO=$(OUTPUT)/isis2stdTruth_argb.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/grayscale.cub \
	  to=$(OUTPUT)/isis2stdTruth_gray_u16bit.jp2 \
	  mode=grayscale \
	  format=jp2 \
	  bittype=u16bit \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray_u16bit.jp2 \
	  TO=$(OUTPUT)/isis2stdTruth_gray_u16bit.cub >/dev/null;
	$(APPNAME) from=$(INPUT)/grayscale.cub \
	  to=$(OUTPUT)/isis2stdTruth_gray_s16bit.jp2 \
	  mode=grayscale \
	  format=jp2 \
	  bittype=s16bit \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray_s16bit.jp2 \
	  TO=$(OUTPUT)/isis2stdTruth_gray_s16bit.cub >/dev/null;
	$(RM) $(OUTPUT)/isis2stdTruth_gray.jp2;
	$(RM) $(OUTPUT)/isis2stdTruth_rgb.jp2;
	$(RM) $(OUTPUT)/isis2stdTruth_argb.jp2;
	$(RM) $(OUTPUT)/isis2stdTruth_gray_u16bit.jp2;
	$(RM) $(OUTPUT)/isis2stdTruth_gray_s16bit.jp2;
