APPNAME = vims2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/CM_1514284191_1.cub \
	vis=$(OUTPUT)/vimsVisTruth.cub \
	ir=$(OUTPUT)/vimsIrTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/vimsVisTruth.cub \
	  to=$(OUTPUT)/vis.pvl > /dev/null;
	catlab from=$(OUTPUT)/vimsIrTruth.cub \
	  to=$(OUTPUT)/ir.pvl > /dev/null;
