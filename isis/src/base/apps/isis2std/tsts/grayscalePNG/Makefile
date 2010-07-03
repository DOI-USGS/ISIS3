APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2stdTruth1.png \
	  mode=grayscale \
	  format=png \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth1.png \
	  TO=$(OUTPUT)/isis2stdTruth1.cub > /dev/null;
	rm -f $(OUTPUT)/isis2stdTruth1.png;
