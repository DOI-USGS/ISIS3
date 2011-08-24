APPNAME = automos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > cubes.lis;
	$(APPNAME) fromlist=cubes.lis \
	  mosaic=$(OUTPUT)/rc3_ir_mos.cub \
	  grange=user minlat=0 maxlat=40 \
	  minlon=75 maxlon=100 > /dev/null; 
	$(RM) cubes.lis;
