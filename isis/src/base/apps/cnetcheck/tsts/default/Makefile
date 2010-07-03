APPNAME = cnetcheck

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis prefix=$(OUTPUT)/cat \
	nolatlon = false \
	cnet=$(INPUT)/NearSide_All.net | grep -v "Processed" \
	> $(OUTPUT)/output.txt;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis prefix=$(OUTPUT)/com \
	ignore=false nolatlon=false delimit=comma \
	cnet=$(INPUT)/NearSide_All.net | grep -v "Processed" \
	> $(OUTPUT)/comoutput.txt;
	$(RM) $(OUTPUT)/comIsland.*
	$(RM) $(OUTPUT)/catIsland.*
	$(RM) $(OUTPUT)/list.lis;
