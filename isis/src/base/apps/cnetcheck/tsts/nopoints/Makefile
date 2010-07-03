APPNAME = cnetcheck

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis prefix=$(OUTPUT)/cat \
	cnet=$(INPUT)/cnet.net | grep -v "Processed" \
	> $(OUTPUT)/output.txt;
	$(RM) $(OUTPUT)/list.lis;
