APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth4.png \
	  mode=grayscale \
	  format=png \
	  stretch=piecewise > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth4.png \
	  MODE=grayscale \
	  TO=$(OUTPUT)/isis2stdTruth4.cub > /dev/null;
	rm -f $(OUTPUT)/isis2stdTruth4.png;
