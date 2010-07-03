APPNAME = handmos

include $(ISISROOT)/make/isismake.tsts

commands:	
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	mosaic=$(OUTPUT)/handmosTruth2.cub \
	priority=beneath \
    insample=1 \
    inline=1 \
    inband=1 \
    outsample=1 \
    outline=2 \
    outband=2 \
    nlines=10 \
	nsamples=10 \
	nbands=5 \
	track=yes \
	create=yes > /dev/null;
