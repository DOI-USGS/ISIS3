APPNAME = handmos

include $(ISISROOT)/make/isismake.tsts

commands:	
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	mosaic=$(OUTPUT)/handmosTruth2.cub \
	priority=BAND \
	insample=1 \
	inline=1 \
	inband=1 \
	outsample=1 \
	outline=1 \
	outband=1 \
	nlines=1000 \
	nsamples=1000 \
	nbands=5 \
	create=yes \
	track=true \
	type=KEYWORD \
	keyname=FilterName \
	keyvalue=RED \
	criteria=LESSER \
	highsaturation=false \
	lowsaturation=false \
	null=false > /dev/null;
