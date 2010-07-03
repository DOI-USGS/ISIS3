APPNAME = footprintmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/fromList.lis;
	$(APPNAME) fromlist=$(OUTPUT)/fromList.lis \
	to=$(OUTPUT)/islands.txt mode=FULL > /dev/null;> /dev/null;
	$(RM) $(OUTPUT)/fromList.lis;
	$(SED) -e 's+\([^b]*\)\(m[0-9]*.lev1.crop.cub\)+\2X+g' \
	  output/islands.txt | $(SED) -e 's+)++' | tr "X" "\n" \
	  > $(OUTPUT)/islands2.txt;
	$(SED) -e 's+Files *=.*-++' $(OUTPUT)/islands2.txt | \
	  $(SED) -e 's+/.*-++g' | $(SED) -e 's+,++g' > $(OUTPUT)/islands.pvl;
	$(RM) $(OUTPUT)/islands.txt $(OUTPUT)/islands2.txt > /dev/null;
