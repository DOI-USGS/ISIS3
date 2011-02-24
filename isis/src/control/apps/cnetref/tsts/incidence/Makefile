APPNAME = cnetref

truth.pvl.IGNORELINES = UserName Created LastModified DateTime

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/autoseed.net \
	  deffile=$(INPUT)/deffile.def \
	  criteria=incidence \
	  onet=$(OUTPUT)/truth.bin \
	  networkid=DefaultTruth \
	  Description=RefByIncidence > /dev/null;
	  cnetbin2pvl from=$(OUTPUT)/truth.bin \
	  to=$(OUTPUT)/truth.pvl >/dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
	  $(RM) $(OUTPUT)/truth.bin;
