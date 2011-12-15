APPNAME = pointreg

log.ignored.txt.IGNORELINES = Processed
log.valid.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;

	# Register Ignored Only
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.ignored.net \
	flatfile=$(OUTPUT)/flatfile.ignored.txt \
	outputignored=no \
	outputfailed=no \
	points=ignored > $(OUTPUT)/log.ignored.txt;

	# Register Valid Only
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.valid.net \
	flatfile=$(OUTPUT)/flatfile.valid.txt \
	outputignored=no \
	outputfailed=no > $(OUTPUT)/log.valid.txt;

	$(RM) $(OUTPUT)/cub.lis;
