APPNAME = cnetref

truth.pvl.IGNORELINES = UserName Created LastModified DateTime

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/autoseed.net \
	  deffile=$(INPUT)/deffile.def \
	  criteria=resolution \
	  type=low \
	  onet=$(OUTPUT)/truth.net \
	  networkid=DefaultTruth \
	  Description=RefByResolutionLow > /dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
