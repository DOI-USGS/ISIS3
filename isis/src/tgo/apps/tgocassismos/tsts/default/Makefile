APPNAME = tgocassismos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/mosaic.lis;
	$(APPNAME) fromlist=$(OUTPUT)/mosaic.lis \
		   to=$(OUTPUT)/tgocassismos.cub >& /dev/null;
	catlab from=$(OUTPUT)/tgocassismos.cub to=$(OUTPUT)/tgocassismos.pvl >& /dev/null;	
	catlab from=$(OUTPUT)/tgocassismos_tracking.cub to=$(OUTPUT)/tgocassismos_tracking.pvl >& /dev/null;
	$(RM) $(OUTPUT)/mosaic.lis;
