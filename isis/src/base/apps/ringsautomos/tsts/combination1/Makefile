# This case tests the following combination of parameters for ringsautomos
#
# tolist is given
# priority=beneath
# groundrange=user minringlon=0	maxringlon=100	minringrad=8000000	maxringrad=100000000	
# track=true
# matchbandbin=false
# matchdem=true
#
APPNAME = ringsautomos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.projected.cub > $(OUTPUT)/fromlist.lis;
	$(APPNAME) from=$(OUTPUT)/fromlist.lis \
	  mosaic=$(OUTPUT)/wacMosaic.cub \
	  tolist=$(OUTPUT)/tolist.txt \
	  priority=beneath \
	  grange=user \
	  minringlon= 0 \
	  maxringlon= 100 \
	  minringrad= 8000000 \
	  maxringrad= 100000000 \
	  track=true \
	  matchbandbin=false \
	  matchdem=true \
	  > /dev/null;
	catlab from=$(OUTPUT)/wacMosaic.cub \
	  to=$(OUTPUT)/wacMosaic.pvl > /dev/null;
	$(SED) 's+/.*input/+input/+' $(OUTPUT)/tolist.txt > $(OUTPUT)/tolist_mod.txt 
	$(RM) $(OUTPUT)/fromlist.lis;
	$(RM) $(OUTPUT)/tolist.txt;
