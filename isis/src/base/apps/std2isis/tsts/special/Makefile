APPNAME = std2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/in.png TO=$(OUTPUT)/out.cub \
	mode=grayscale \
        setnullrange=yes nullmin=100 nullmax=240 sethrsrange=yes \
	hrsmin=200 hrsmax=260 setlrsrange=yes lrsmin=0 lrsmax=100 \
	> /dev/null;
