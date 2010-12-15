APPNAME = pointreg

log.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/input.net \
	deffile=$(INPUT)/definitions.def \
	onet=$(OUTPUT)/output.net \
	flatfile=$(OUTPUT)/flat.txt \
	outputignored=no outputfailed=no \
	points=all measures=all > $(OUTPUT)/log.txt;
	$(RM) $(OUTPUT)/cub.lis;
