APPNAME = nocam2map

nocam2maptruth.cub.TOLERANCE = .0002

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/frt_61b2_trdr.cub+400 \
	latcub=$(INPUT)/frt_61b2_ddr.cub+4 \
	loncub=$(INPUT)/frt_61b2_ddr.cub+5 \
	to=$(OUTPUT)/nocam2maptruth.cub > /dev/null;
