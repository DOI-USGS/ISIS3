APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2stdTruth3.png \
	  mode=grayscale \
	  format=png \
	  stretch=manual \
	  minimum = -1e20 \
	  maximum = 1e20 > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth3.png \
	  TO=$(OUTPUT)/isis2stdTruth3.cub > /dev/null;
	rm -f $(OUTPUT)/isis2stdTruth3.png; 
