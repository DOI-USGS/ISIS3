APPNAME = fx

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) to=$(OUTPUT)/OUTPUTONLYresult.cub \
	mode=outputonly \
	lines=100 \
	samples=100 \
	bands=2 \
	equation=line+sample+band > /dev/null;
