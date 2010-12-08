APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	find $(INPUT)/*.cub > $(OUTPUT)/cubeList.lst; \
	$(APPNAME) fromlist=$(OUTPUT)/cubeList.lst \
	cnet=$(INPUT)/testNet.net \
	to=$(OUTPUT)/cnetstats.txt \
	allowErrors = true \
	> /dev/null;
	rm -f $(OUTPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/cnetstats.txt | sed s/,\[^,]\*\.cub,/,file.cub,/ \
	 | sed 's/\([0-9][0-9]*\.[0-9]\)\([0-9][0-9]*\)/\1/g' \
	   > $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/cnetstats.txt > /dev/null;
