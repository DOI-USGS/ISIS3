APPNAME = pointreg

redPointregLog.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;

	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/redOutputCnet.net \
	flatfile=$(OUTPUT)/redFlatFile.csv \
	validate=after falsepositives=$(OUTPUT)/falsepositives.csv \
	revert=no shift=0.1 \
	points=nonignored > $(OUTPUT)/redPointregLog.txt;

	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(OUTPUT)/redOutputCnet.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnetReverted.net \
	flatfile=$(OUTPUT)/flatfileReverted.csv \
	validate=only falsepositives=$(OUTPUT)/fpReverted.csv \
	shift=0.1 \
	points=nonignored > $(OUTPUT)/logReverted.txt;

	$(RM) $(OUTPUT)/cub.lis;
