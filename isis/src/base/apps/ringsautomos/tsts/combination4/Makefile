# This case tests the following combination of parameters for ringsautomos
#
# priority=band
# type=keyword
# keywordname=
# keywordvalue=
# criteria=greater
#
APPNAME = ringsautomos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.projected.cub > $(OUTPUT)/fromlist.lis;
	$(APPNAME) from=$(OUTPUT)/fromlist.lis \
	  mosaic=$(OUTPUT)/vimsIrMosaic.cub \
	  priority=band \
	  type=keyword \
	  keyname=OriginalBand \
	  keyvalue=98 \
	  criteria=greater \
	  > /dev/null;
	catlab from=$(OUTPUT)/vimsIrMosaic.cub \
	  to=$(OUTPUT)/vimsIrMosaic.pvl > /dev/null;
	$(RM) $(OUTPUT)/fromlist.lis;
