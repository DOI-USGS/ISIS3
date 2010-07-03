APPNAME = pointreg

redPointregLog.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(APPNAME) files=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	template=$(INPUT)/autoRegTemplate.def \
	to=$(OUTPUT)/redOutputCnet.pvl \
	flatfile=$(OUTPUT)/redFlatFile.txt \
	points=nonignored > $(OUTPUT)/redPointregLog.txt;
	$(RM) $(OUTPUT)/cub.lis;
