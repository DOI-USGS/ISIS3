APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis cnet=$(INPUT)/cnet.net \
	  tolist=$(OUTPUT)/newList.lis onet=$(OUTPUT)/newNet.net \
	  networkid=new description=new prefix=$(OUTPUT)/pre \
	  noignore=true nosinglemeasure=true nomeasureless=true \
	  > /dev/null;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
