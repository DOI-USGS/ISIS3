APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/appendTo.txt $(OUTPUT); \
	find $(INPUT)/*.cub > $(INPUT)/cubeList.lst; \
	$(APPNAME) fromlist=$(INPUT)/cubeList.lst \
	cnet=$(INPUT)/testNet.net \
	to=$(OUTPUT)/appendTo.txt \
        append=true > /dev/null; 
	rm -f $(INPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/appendTo.txt | sed s/,\[^,]\*\.cub,/,.cub,/ \
	> $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/appendTo.txt
