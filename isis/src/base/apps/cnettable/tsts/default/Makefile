APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	find $(INPUT)/*.cub > $(INPUT)/cubeList.lst; \
	$(APPNAME) fromlist=$(INPUT)/cubeList.lst \
	cnet=$(INPUT)/testNet.net \
	to=$(OUTPUT)/cnetstats.txt \
        > /dev/null;
	rm -f $(INPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/cnetstats.txt | sed s/,\[^,]\*\.cub,/,.cub,/ \
	> $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/cnetstats.txt > /dev/null;
