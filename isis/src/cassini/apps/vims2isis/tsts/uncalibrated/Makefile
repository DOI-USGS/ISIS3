APPNAME = vims2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/v1545949478_1.qub \
	vis=$(OUTPUT)/vimsVisTruth.cub \
        ir=$(OUTPUT)/vimsIrTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/vimsVisTruth.cub \
	  to=$(OUTPUT)/vis.pvl > /dev/null;
	catlab from=$(OUTPUT)/vimsIrTruth.cub \
	  to=$(OUTPUT)/ir.pvl > /dev/null;
