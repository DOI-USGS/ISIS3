APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis cnet=$(INPUT)/cnet.net \
	  tolist=$(OUTPUT)/newList.lis onet=$(OUTPUT)/newNetPoint.net \
	  networkid=new description=new prefix=$(OUTPUT)/point \
	  noignore=true \
	  cubes=true cubelist=$(OUTPUT)/list.lis > /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis cnet=$(INPUT)/cnet.net \
	  tolist=$(OUTPUT)/newList.lis onet=$(OUTPUT)/newNetMeasure.net \
	  networkid=new description=new prefix=$(OUTPUT)/measure \
	  noignore=true \
	  cubes=true cubelist=$(OUTPUT)/list.lis cubemeasures=true retain_reference=true > /dev/null;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
