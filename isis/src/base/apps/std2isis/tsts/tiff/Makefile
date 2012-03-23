APPNAME = std2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/grayscale.tif TO=$(OUTPUT)/grayscale.cub \
        > /dev/null;

	$(APPNAME) FROM=$(INPUT)/rgb.tif TO=$(OUTPUT)/rgb.cub \
        > /dev/null;
