APPNAME = pointreg

log.default.txt.IGNORELINES = Processed
log.revert.txt.IGNORELINES = Processed
log.skipped.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;

	# Test the validation without reverting failed registration
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/cnet.default.net \
	flatfile=$(OUTPUT)/flatfile.default.csv \
	validate=after falsepositives=$(OUTPUT)/falsepos.default.csv \
	revert=no shift=0.1 \
	points=nonignored > $(OUTPUT)/log.default.txt;

	# Test the validation with reverting failed registration
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(OUTPUT)/cnet.default.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/cnet.revert.net \
	flatfile=$(OUTPUT)/flatfile.revert.csv \
	validate=only falsepositives=$(OUTPUT)/falsepos.revert.csv \
	shift=0.1 search=7 \
	points=nonignored > $(OUTPUT)/log.revert.txt;

	# Test the validation with everything being skipped due to an absurdly strict
	# RESTOLERANCE of 0.0 (i.e., nothing should be reverted, even though we enable
	# reverting as an option)
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(OUTPUT)/cnet.default.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/cnet.skipped.net \
	flatfile=$(OUTPUT)/flatfile.skipped.csv \
	validate=only falsepositives=$(OUTPUT)/falsepos.skipped.csv \
	restolerance=0.0 shift=0.1 search=7 \
	points=nonignored > $(OUTPUT)/log.skipped.txt;

	$(RM) $(OUTPUT)/cub.lis;
