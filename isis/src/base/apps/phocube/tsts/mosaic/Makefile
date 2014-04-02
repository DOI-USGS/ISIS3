APPNAME = phocube

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/bib_i3.cub to=$(OUTPUT)/bib_i3_pho.cub\
	phase=no emission=no incidence=no >& $(OUTPUT)/errors.txt || true;
