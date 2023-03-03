APPNAME = tgocassisstitch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/stitch.lis;
	$(APPNAME) fromlist=$(OUTPUT)/stitch.lis outputprefix=bada. outputsuffix=.boom cubename=testo;
	$(RM) $(OUTPUT)/stitch.lis;
    $(RM) $(OUTPUT)/bada.testo.boom.cub
