APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	find $(INPUT)/*.cub > $(INPUT)/cubeList.lst; \
	$(APPNAME) fromlist=$(INPUT)/cubeList.lst \
	cnet=$(INPUT)/testNet.net \
	flatfile=$(OUTPUT)/cnetstats.txt \
	allowErrors = true \
	> /dev/null;
	rm -f $(INPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/cnetstats.txt | sed s/,\[^,]\*\.cub,/,file.cub,/ \
	 | sed 's/\([0-9][0-9]*\.[0-9]\)\([0-9][0-9]*\)/\1/g' \
	   > $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/cnetstats.txt > /dev/null;
