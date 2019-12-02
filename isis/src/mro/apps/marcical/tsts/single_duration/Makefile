APPNAME = marcical

include $(ISISROOT)/make/isismake.tsts
truth_iof.cub.TOLERANCE = 0.0000000000001
truth_not_iof.cub.TOLERANCE = 0.0000000000001

commands:
	$(APPNAME) FROM=$(INPUT)/K14_059003_3475_MA_00N112W.IMG.even.crop.cub \
	  TO=$(OUTPUT)/truth_iof.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/K14_059003_3475_MA_00N112W.IMG.even.crop.cub iof=no \
	  TO=$(OUTPUT)/truth_not_iof.cub > /dev/null;
