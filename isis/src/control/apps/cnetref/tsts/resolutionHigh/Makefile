APPNAME = cnetref

truth.pvl.IGNORELINES = UserName Created LastModified DateTime

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/autoseed.net \
	  deffile=$(INPUT)/deffile.def \
	  criteria=resolution \
	  type=high \
	  onet=$(OUTPUT)/truth.net \
	  networkid=DefaultTruth \
	  Description=RefByResolutionHigh > /dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(INPUT)/apolloList.lis \
	  cnet=$(INPUT)/apolloSeed.net \
	  criteria=resolution \
	  type=high \
	  onet=$(OUTPUT)/apolloTruth.net > /dev/null;
# cnetref fromlist=/home/acpaquette/REV71.lis cnet=/home/acpaquette/AS15-REV71_Add-SPLT10.net onet=/home/acpaquette/temp.net criteria=resolution