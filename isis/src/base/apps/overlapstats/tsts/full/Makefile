APPNAME = overlapstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) from=$(OUTPUT)/list.lis \
	           to=$(OUTPUT)/csv.txt \
			   OVERLAPLIST=$(INPUT)/overlaps.lis \
	           DETAIL=full \
			   SINGLELINE=false > /dev/null;
	$(CAT) $(OUTPUT)/csv.txt | \
	       $(SED) 's:/[^,]*.lev1.cub:PATH:g' | \
	       $(SED) 's/\([0-9]\.[0-9]\{3\}\)\([0-9]*\)/\1/g' | \
	       $(SED) 's/\([0-9]\{6\}[0-9]*\)\(\.[0-9][0-9]*\)/\1/g' \
	       > $(OUTPUT)/csvNoPaths.txt;
	$(RM) $(OUTPUT)/csv.txt;

	$(APPNAME) from=$(OUTPUT)/list.lis \
	           to=$(OUTPUT)/tab.txt \
			   OVERLAPLIST=$(INPUT)/overlaps.lis \
	           DETAIL=full \
			   TABLETYPE=tab > /dev/null;
	$(CAT) $(OUTPUT)/tab.txt | tr '\t' ',' | \
	       $(SED) 's:/[^,]*.lev1.cub:PATH:g' | \
	       $(SED) 's/\([0-9]\.[0-9]\{3\}\)\([0-9]*\)/\1/g' | \
	       $(SED) 's/\([0-9]\{6\}[0-9]*\)\(\.[0-9][0-9]*\)/\1/g' | \
	       tr ',' '\t' \
	       > $(OUTPUT)/tabNoPaths.txt;
	$(RM) $(OUTPUT)/tab.txt;

	$(APPNAME) from=$(OUTPUT)/list.lis \
	           to=$(OUTPUT)/csvDefault.txt \
			   OVERLAPLIST=$(INPUT)/overlaps.lis \
	           DETAIL=full > /dev/null;
	$(CAT) $(OUTPUT)/csvDefault.txt | tr '\t' ',' | \
	       $(SED) 's:/[^,]*.lev1.cub:PATH:g' | \
	       $(SED) 's/\([0-9]\.[0-9]\{3\}\)\([0-9]*\)/\1/g' | \
	       $(SED) 's/\([0-9]\{6\}[0-9]*\)\(\.[0-9][0-9]*\)/\1/g' \
	       > $(OUTPUT)/csvDefaultNoPaths.txt;
	$(RM) $(OUTPUT)/csvDefault.txt;
	$(RM) $(OUTPUT)/list.lis;
