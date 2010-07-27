APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/appendTo.txt $(OUTPUT); \
	find $(INPUT)/*.cub > $(OUTPUT)/cubeList.lst; \
	$(APPNAME) fromlist=$(OUTPUT)/cubeList.lst \
	cnet=$(INPUT)/testNet.net \
	to=$(OUTPUT)/appendTo.txt \
        append=true > /dev/null; 
	rm -f $(OUTPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/appendTo.txt | sed s/,\[^,]\*\.cub,/,.cub,/ \
	> $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/appendTo.txt
