APPNAME = pointreg

redPointregLog.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/redOutputCnet.net \
	flatfile=$(OUTPUT)/redFlatFile.txt \
	outputfailed=no \
	points=nonignored > $(OUTPUT)/redPointregLog.txt;
	$(RM) $(OUTPUT)/cub.lis;
