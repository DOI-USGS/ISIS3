APPNAME = voy2isis

include $(ISISROOT)/make/isismake.tsts

# This is just to check that the reading of the Wide Angle data works
# after fixing bug m04561
# The output file was assumed correct.
commands:
	$(APPNAME) from=$(INPUT)/in1.imq \
	to=$(OUTPUT)/in1.truth.cub > /dev/null;
	
	catlab from=$(OUTPUT)/in1.truth.cub \
	to=$(OUTPUT)/in1.truth.pvl > /dev/null;
