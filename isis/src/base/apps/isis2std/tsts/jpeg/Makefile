# Test JPEG images
# Test 1: Grayscale from first band of isisTruth cube using linear stretch. The output truth cube 
#         from this test should be identical to the output truth cubes isis2stdTruth
#         for grayscale test cases of bmp, jpeg2000, png, and tiff
APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth_gray.bmp \
	  mode=grayscale \
	  format=bmp \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray.bmp \
	  TO=$(OUTPUT)/isis2stdTruth_gray.cub >/dev/null;
	$(RM) $(OUTPUT)/isis2stdTruth_gray.bmp;
