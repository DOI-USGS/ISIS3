# Test for ringsmappt projection (interprets coordinate as (x,y) )
APPNAME = ringsmappt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/userEnteredXY.pvl \
	  format=pvl \
	  append=no \
	  type=projection \
	  x=-58752169.950728 \
	  y=-83725336.55945 > /dev/null;
