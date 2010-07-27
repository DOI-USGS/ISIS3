APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	find $(INPUT)/*.cub > $(OUTPUT)/cubeList.lst; \
	$(APPNAME) fromlist=$(OUTPUT)/cubeList.lst \
	cnet=$(INPUT)/testNet.net \
	to=$(OUTPUT)/cnetstats.txt \
        > /dev/null;
	rm -f $(OUTPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/cnetstats.txt | sed s/,\[^,]\*\.cub,/,.cub,/ \
	> $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/cnetstats.txt > /dev/null;
