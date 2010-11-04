APPNAME = cnetref

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/autoseed.net \
	  deffile=$(INPUT)/deffile.def \
	  criteria=emission \
	  onet=$(OUTPUT)/truth.pvl \
	  networkid=DefaultTruth \
	  Description=RefByEmission > /dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
