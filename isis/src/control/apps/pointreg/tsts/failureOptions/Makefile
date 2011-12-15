APPNAME = pointreg

logKeep.txt.IGNORELINES = Processed
logRemove.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	
	# Keep Failures
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/input.net \
	deffile=$(INPUT)/definitions.def \
	onet=$(OUTPUT)/outputKeep.net \
	flatfile=$(OUTPUT)/flatKeep.txt \
	outputignored=no \
	points=all measures=all > $(OUTPUT)/logKeep.txt;
	
	# Remove Failures
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/input.net \
	deffile=$(INPUT)/definitions.def \
	onet=$(OUTPUT)/outputRemove.net \
	flatfile=$(OUTPUT)/flatRemove.txt \
	outputignored=no outputfailed=no \
	points=all measures=all > $(OUTPUT)/logRemove.txt;
	
	$(RM) $(OUTPUT)/cub.lis;
