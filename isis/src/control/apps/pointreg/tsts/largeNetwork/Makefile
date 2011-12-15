APPNAME = pointreg

log.txt.IGNORELINES = Processed

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/inputcontrol.net \
	deffile=$(INPUT)/autoRegTemplate.def \
	onet=$(OUTPUT)/outnet.net \
	flatfile=$(OUTPUT)/flatfile.txt \
	measures=candidates > $(OUTPUT)/log.txt;
	$(RM) $(OUTPUT)/cub.lis;
