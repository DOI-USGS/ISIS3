APPNAME = pointreg

outnet.pvl.IGNORELINES = DateTime

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(APPNAME) files=$(OUTPUT)/cub.lis \
	cnet=$(INPUT)/inputcontrol.net \
	template=$(INPUT)/autoRegTemplate.def \
	to=$(OUTPUT)/outnet.pvl \
	flatfile=$(OUTPUT)/flatfile.txt \
	measures=estimated \
	> /dev/null;
	$(RM) $(OUTPUT)/cub.lis;
