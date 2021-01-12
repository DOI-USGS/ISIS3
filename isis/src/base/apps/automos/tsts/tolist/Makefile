APPNAME = automos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > cubes.lis;
	$(APPNAME) fromlist=cubes.lis \
	  mosaic=$(OUTPUT)/rc3_ir_mos.cub \
	  tolist=$(OUTPUT)/outlist1.txt \
	  grange=user minlat=0 maxlat=40 \
	  minlon=75 maxlon=100 > /dev/null;	
	cat $(OUTPUT)/outlist1.txt | \
	sed 's|^.*input|input|' > $(OUTPUT)/outlist.txt;
	$(RM) cubes.lis;
	$(RM) $(OUTPUT)/rc3_ir_mos.cub;
	$(RM) $(OUTPUT)/outlist1.txt;
