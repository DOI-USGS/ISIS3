APPNAME = photomet

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0031257484M.iof.cub \
	to=$(OUTPUT)/EN0031257484M.phot.cub \
	phopar=$(INPUT)/pvlfile.txt \
	maxincidence=75 > /dev/null;
