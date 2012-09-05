APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) red=$(INPUT)/rgb.cub+1 \
	  green=$(INPUT)/rgb.cub+2 \
	  blue=$(INPUT)/rgb.cub+3 \
	  to=$(OUTPUT)/rgb.tif \
	  mode=rgb \
	  format=tiff \
	  stretch=linear > /dev/null; \
	cat $(OUTPUT)/rgb.tfw | sed 's/\(\.[0-9][0-9][0-9]\)\([0-9]*\)/\1/' \
	  > $(OUTPUT)/rgb.txt; \
	$(RM) $(OUTPUT)/rgb.tfw; \
	std2isis FROM=$(OUTPUT)/rgb.tif \
	  TO=$(OUTPUT)/rgb.cub > /dev/null; \
	rm -rf $(OUTPUT)/rgb.tif; \
	$(APPNAME) from=$(INPUT)/grayscale.cub \
	  to=$(OUTPUT)/grayscale.tif \
	  mode=grayscale \
	  format=tiff \
	  bittype=u16bit \
	  stretch=linear > /dev/null; \
	std2isis FROM=$(OUTPUT)/grayscale.tif \
	  TO=$(OUTPUT)/grayscale.cub > /dev/null; \
	rm -rf $(OUTPUT)/grayscale.tif;
