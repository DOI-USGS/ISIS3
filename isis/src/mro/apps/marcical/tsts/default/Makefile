APPNAME = marcical

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/crop.cub \
	  TO=$(OUTPUT)/truth_default.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/crop.cub iof=no \
	  TO=$(OUTPUT)/truth_not_iof.cub > /dev/null;
