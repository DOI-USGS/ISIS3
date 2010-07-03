APPNAME = himos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
		   to=$(OUTPUT)/test_himos_070418.cub > /dev/null;
	catlab from=$(OUTPUT)/test_himos_070418.cub > $(OUTPUT)/himos.pvl;
	$(RM) $(OUTPUT)/cube.lis;
