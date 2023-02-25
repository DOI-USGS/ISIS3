APPNAME = tgocassisstitch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/stitch.lis;
	$(APPNAME) fromlist=$(OUTPUT)/stitch.lis out=testxyza suffix=true;
	$(RM) $(OUTPUT)/stitch.lis;
    $(RM) $(OUTPUT)/*testxyza.cub
