APPNAME = himos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
		   to=$(OUTPUT)/test_himos_pole.cub > /dev/null;
	catlab from=$(OUTPUT)/test_himos_pole.cub > $(OUTPUT)/himos_pole.pvl;
	$(RM) $(OUTPUT)/cube.lis;
