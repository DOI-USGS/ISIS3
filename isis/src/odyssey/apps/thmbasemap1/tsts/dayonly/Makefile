APPNAME = thmbasemap1

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/proc.lis uselist=yes to=$(OUTPUT) night=false \
	atmoscorr=false info=yes tostat=$(OUTPUT)/tostat.txt footprint=false rminput=false \
	rmhighinc=yes highinclist=$(OUTPUT)/highinclist.txt nofilelist=$(OUTPUT)/nofilelist.txt \
	nadirspclist=$(OUTPUT)/nadirspclist.txt imagegaplist=$(OUTPUT)/imagegaplist.txt \
	> /dev/null;
	cat $(OUTPUT)/tostat.txt | \
	sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	> $(OUTPUT)/tostat_new.txt;
	$(MV) $(OUTPUT)/tostat_new.txt $(OUTPUT)/tostat.txt;
