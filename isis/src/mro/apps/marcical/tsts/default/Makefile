APPNAME = marcical

include $(ISISROOT)/make/isismake.tsts
truth_default.cub.TOLERANCE = 0.0000000000001
truth_not_iof.cub.TOLERANCE = 0.0000000000001

commands:
	$(APPNAME) FROM=$(INPUT)/P12_005901_3391_MA_00N096W.even.crop.cub \
	  TO=$(OUTPUT)/truth_default.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/P12_005901_3391_MA_00N096W.even.crop.cub iof=no \
	  TO=$(OUTPUT)/truth_not_iof.cub > /dev/null;
