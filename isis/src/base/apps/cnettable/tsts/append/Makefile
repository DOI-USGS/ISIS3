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
	cat $(OUTPUT)/appendTo.txt | sed s/,\[^,]\*\.cub,/,file.cub,/ \
	  | sed 's/\([0-9][0-9]*\.[0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/appendTo.txt
