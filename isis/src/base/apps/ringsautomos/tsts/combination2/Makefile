# This case tests the following combination of parameters for ringsautomos
#
# priority=average
# hisat=true
# losat=true
# null=true
#
APPNAME = ringsautomos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.projected.cub > $(OUTPUT)/fromlist.lis;
	$(APPNAME) from=$(OUTPUT)/fromlist.lis \
	  mosaic=$(OUTPUT)/wacMosaic.cub \
	  priority=average \
	  highsat=true \
	  lowsat=true \
	  null=true \
	  > /dev/null;
	catlab from=$(OUTPUT)/wacMosaic.cub \
	  to=$(OUTPUT)/wacMosaic.pvl > /dev/null;
	$(RM) $(OUTPUT)/fromlist.lis;
