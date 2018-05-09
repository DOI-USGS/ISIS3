APPNAME = tgocassismos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
		   to=$(OUTPUT)/test_tgocassismos_070418.cub > /dev/null;
	catlab from=$(OUTPUT)/test_tgocassismos_070418.cub to=$(OUTPUT)/tgocassismos.pvl > /dev/null;
	$(RM) $(OUTPUT)/cube.lis;
