APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth2.bmp \
	  mode=grayscale \
	  format=bmp \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth2.bmp \
	  TO=$(OUTPUT)/isis2stdTruth2.cub >/dev/null;
	rm -f $(OUTPUT)/isis2stdTruth2.bmp;
