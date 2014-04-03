APPNAME = vimscal

V1483214528_1.ir.avgdark.cal.cub.TOLERANCE = 0.000000001
V1483214528_1.ir.origdark.cal.cub.TOLERANCE = 0.00000000004

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/V1483214528_1.ir.cub \
	  TO=$(OUTPUT)/V1483214528_1.ir.avgdark.cal.cub \
	  irorigdark=false > /dev/null;
	catlab from=$(OUTPUT)/V1483214528_1.ir.avgdark.cal.cub \
	  TO=$(OUTPUT)/V1483214528_1.ir.avgdark.cal.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/V1483214528_1.ir.cub \
	  TO=$(OUTPUT)/V1483214528_1.ir.origdark.cal.cub \
	  irorigdark=true > /dev/null;
	catlab from=$(OUTPUT)/V1483214528_1.ir.origdark.cal.cub \
	  TO=$(OUTPUT)/V1483214528_1.ir.origdark.cal.pvl > /dev/null;
