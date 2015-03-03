APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/temp.csv \
	format=FLAT coordtype=IMAGE \
	append=false > /dev/null;
	cut -d , -f 2- $(OUTPUT)/temp.csv >& $(OUTPUT)/pointlistFlatTruth.csv;
	rm $(OUTPUT)/temp.csv;
