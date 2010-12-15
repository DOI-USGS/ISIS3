APPNAME = cnetcheck

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis prefix=$(OUTPUT)/cat \
	nocube=false \
	cnet=$(INPUT)/NearSide_All.net | grep -v "Processed" \
	> $(OUTPUT)/output.txt;
	$(RM) $(OUTPUT)/list.lis;
	$(RM) $(OUTPUT)/catIsland.*
