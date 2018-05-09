APPNAME = tgocassismos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
		   to=$(OUTPUT)/test_tgocassismos_pole.cub > /dev/null;
	catlab from=$(OUTPUT)/test_tgocassismos_pole.cub to=$(OUTPUT)/tgocassismos_pole.pvl > /dev/null;
	$(RM) $(OUTPUT)/cube.lis;
