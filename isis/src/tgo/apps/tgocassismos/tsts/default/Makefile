APPNAME = tgocassismos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/mosaic.lis;
	$(APPNAME) fromlist=$(OUTPUT)/mosaic.lis \
		   to=$(OUTPUT)/tgocassismos.cub ;
	catlab from=$(OUTPUT)/tgocassismos.cub to=$(OUTPUT)/tgocassismos.pvl;	
	$(RM) $(OUTPUT)/mosaic.lis;
