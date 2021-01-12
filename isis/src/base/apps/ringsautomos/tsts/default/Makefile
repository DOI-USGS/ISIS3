# This case tests the following default parameters for ringsautomos
#
# tolist=None
# priority=ontop
# groundrange=auto
# track=false
# matchbandbin=true
# matchdem=false
# highsat=false
# lowsat=false
# null=false
#
APPNAME = ringsautomos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.projected.cub > $(OUTPUT)/fromlist.lis;
	$(APPNAME) from=$(OUTPUT)/fromlist.lis \
	  mosaic=$(OUTPUT)/wacMosaic.cub > /dev/null;
	catlab from=$(OUTPUT)/wacMosaic.cub \
	  to=$(OUTPUT)/wacMosaic.pvl > /dev/null;
	$(RM) $(OUTPUT)/fromlist.lis;
