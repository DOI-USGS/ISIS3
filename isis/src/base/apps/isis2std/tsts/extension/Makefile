# Test BMP images
# Test 1: Test that isis2std adds the appropriate extension based 
#         on the given format to the output file.
APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth_gray \
	  mode=grayscale \
	  format=bmp \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth_gray.bmp \
	  TO=$(OUTPUT)/isis2stdTruth_gray.cub >/dev/null;
	$(RM) $(OUTPUT)/isis2stdTruth_gray.bmp;
