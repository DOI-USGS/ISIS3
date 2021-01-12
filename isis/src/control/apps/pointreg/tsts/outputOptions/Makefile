APPNAME = pointreg

log.a.txt.IGNORELINES = Processed
log.b.txt.IGNORELINES = Processed
log.c.txt.IGNORELINES = Processed
log.d.txt.IGNORELINES = Processed
log.e.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;

	# A. Output Ignored, Register Ignored
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.a.net \
	flatfile=$(OUTPUT)/flatfile.a.txt \
	outputfailed=no \
	points=ignored > $(OUTPUT)/log.a.txt;

	# B. Output Ignored, Unmeasured, Register Ignored
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.b.net \
	flatfile=$(OUTPUT)/flatfile.b.txt \
	points=ignored \
	measures=candidates > $(OUTPUT)/log.b.txt;

	# C. Output Ignored Only
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.c.net \
	flatfile=$(OUTPUT)/flatfile.c.txt \
	outputfailed=no \
	points=nonignored > $(OUTPUT)/log.c.txt;

	# D. Output Unmeasured Only
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.d.net \
	flatfile=$(OUTPUT)/flatfile.d.txt \
	outputignored=no \
	points=nonignored > $(OUTPUT)/log.d.txt;

	# E. Output Unmeasured, Register Ignored
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/red.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.e.net \
	flatfile=$(OUTPUT)/flatfile.e.txt \
	outputignored=no \
	points=ignored > $(OUTPUT)/log.e.txt;

	$(RM) $(OUTPUT)/cub.lis;
