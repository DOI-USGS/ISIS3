APPNAME = pointreg

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(APPNAME) files=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	template=$(INPUT)/autoRegTemplate.def \
	to=$(OUTPUT)/redOutputCnet.pvl \
	flatfile=$(OUTPUT)/redFlatFile.txt \
	points=ignored \
	measures=estimated >& $(OUTPUT)/redPointregLog.txt \
	> /dev/null;
	$(RM) $(OUTPUT)/cub.lis;
