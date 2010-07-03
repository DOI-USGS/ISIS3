APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) red=$(INPUT)/isisTruth.cub+1 \
	  green=$(INPUT)/isisTruth.cub+1 \
	  blue=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2stdTruth5.png \
	  mode=rgb \
	  format=png \
	  stretch=linear > /dev/null;
	std2isis FROM=$(OUTPUT)/isis2stdTruth5.png \
	  TO=$(OUTPUT)/isis2stdTruth5.cub > /dev/null;
	rm -rf $(OUTPUT)/isis2stdTruth5.png;
