APPNAME = nocam2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/trdr9x9.cub \
	latcub=$(INPUT)/ddr9x9.cub+1 \
	loncub=$(INPUT)/ddr9x9.cub+2 \
	to=$(OUTPUT)/nocam2maptruth.cub > /dev/null;

