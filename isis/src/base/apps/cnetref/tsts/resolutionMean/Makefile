APPNAME = cnetref

truth.pvl.IGNORELINES = UserName Created LastModified DateTime

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  network=$(INPUT)/autoseed.net \
	  deffile=$(INPUT)/deffile.def \
	  criteria=resolution \
	  type=mean \
	  to=$(OUTPUT)/truth.pvl \
	  networkid=DefaultTruth \
	  Description=RefByIncidence > /dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
