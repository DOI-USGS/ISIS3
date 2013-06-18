# Test creating PNG images
#
# Test 1: Grayscale from first band of isisTruth cube using linear stretch. The output truth cube 
#         from this test should be identical to the output truth cubes isis2stdTruth
#         for grayscale test cases of bmp, jpeg, jpeg2000, and tiff
#
# Test 2: Grayscale, manual
#
# Test 3: Grayscale, piecewise
#
# Test 4: RGB, manual
#
# Test 5: ARGB
APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth_gray.png \
	  mode=grayscale \
	  format=png \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray.png \
	  TO=$(OUTPUT)/isis2stdTruth_gray.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2stdTruth_gray_manual.png \
	  mode=grayscale \
	  format=png \
	  stretch=manual \
	  minimum = -1e20 \
	  maximum = 1e20 > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray_manual.png \
	  TO=$(OUTPUT)/isis2stdTruth_gray_manual.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth_gray_piecewise.png \
	  mode=grayscale \
	  format=png \
	  stretch=piecewise > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray_piecewise.png \
	  MODE=grayscale \
	  TO=$(OUTPUT)/isis2stdTruth_gray_piecewise.cub > /dev/null;
	$(APPNAME) red=$(INPUT)/isisTruth.cub+1 \
	  green=$(INPUT)/isisTruth.cub+1 \
	  blue=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2stdTruth_rgb_manual.png \
	  mode=rgb \
	  format=png \
	  stretch=manual \
	  rmin=-1e20 \
	  rmax= 1e20 \
	  gmin=-1e20 \
	  gmax= 1e20 \
	  bmin=-1e20 \
	  bmax= 1e20 > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_rgb_manual.png \
	  TO=$(OUTPUT)/isis2stdTruth_rgb_manual.cub > /dev/null;
	$(APPNAME) alpha=$(INPUT)/isisTruth.cub+1 \
	  red=$(INPUT)/isisTruth.cub+1 \
	  green=$(INPUT)/isisTruth.cub+1 \
	  blue=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2stdTruth_argb.png \
	  mode=argb \
	  format=png \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_argb.png \
	  TO=$(OUTPUT)/isis2stdTruth_argb.cub > /dev/null;
	$(RM) $(OUTPUT)/isis2stdTruth_gray.png;
	$(RM) $(OUTPUT)/isis2stdTruth_gray_manual.png; 
	$(RM) $(OUTPUT)/isis2stdTruth_gray_piecewise.png;
	$(RM) $(OUTPUT)/isis2stdTruth_rgb_manual.png;
	$(RM) $(OUTPUT)/isis2stdTruth_argb.png;
