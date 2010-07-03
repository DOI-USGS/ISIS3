APPNAME = overlapstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) from=$(OUTPUT)/list.lis \
	to=$(OUTPUT)/csv.txt OVERLAPLIST=$(INPUT)/overlaps.lis \
	DETAIL=full SINGLELINE=false > /dev/null;
	sed -e 's:/[a-zA-Z/0-9]*.lev1.cub:PATH:g' $(OUTPUT)/csv.txt \
	> $(OUTPUT)/csvNoPaths.txt;
	$(RM) $(OUTPUT)/csv.txt;
	$(APPNAME) from=$(OUTPUT)/list.lis \
	to=$(OUTPUT)/tab.txt OVERLAPLIST=$(INPUT)/overlaps.lis \
	DETAIL=full TABLETYPE=tab > /dev/null;
	sed -e 's:/[a-zA-Z/0-9]*.lev1.cub:PATH:g' $(OUTPUT)/tab.txt \
	> $(OUTPUT)/tabNoPaths.txt;
	$(RM) $(OUTPUT)/tab.txt;
	$(APPNAME) from=$(OUTPUT)/list.lis \
	to=$(OUTPUT)/csvDefault.txt OVERLAPLIST=$(INPUT)/overlaps.lis \
	DETAIL=full > /dev/null;
	sed -e 's:/[a-zA-Z/0-9]*.lev1.cub:PATH:g' $(OUTPUT)/csvDefault.txt \
	> $(OUTPUT)/csvDefaultNoPaths.txt;
	$(RM) $(OUTPUT)/csvDefault.txt;
	$(RM) $(OUTPUT)/list.lis;
