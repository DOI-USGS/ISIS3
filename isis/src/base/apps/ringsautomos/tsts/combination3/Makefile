# This case tests the following combination of parameters for ringsautomos
#
# priority=band
# type=bandnumber
# number=1
# criteria=lesser
#
APPNAME = ringsautomos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.projected.cub > $(OUTPUT)/fromlist.lis;
	$(APPNAME) from=$(OUTPUT)/fromlist.lis \
	  mosaic=$(OUTPUT)/wacMosaic.cub \
	  priority=band \
	  type=bandnumber \
	  number=1 \
	  criteria=lesser \
	  > /dev/null;
	catlab from=$(OUTPUT)/wacMosaic.cub \
	  to=$(OUTPUT)/wacMosaic.pvl > /dev/null;
	$(RM) $(OUTPUT)/fromlist.lis;
