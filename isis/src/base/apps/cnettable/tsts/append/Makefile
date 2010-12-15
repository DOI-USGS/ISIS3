APPNAME = cnettable

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/appendTo.txt $(OUTPUT); \
	find $(INPUT)/*.cub > $(INPUT)/cubeList.lst; \
	$(APPNAME) fromlist=$(INPUT)/cubeList.lst \
	cnet=$(INPUT)/testNet.net \
	flatfile=$(OUTPUT)/appendTo.txt \
	  append=true > /dev/null; 
	rm -f $(INPUT)/cubeList.lst > /dev/null;
	cat $(OUTPUT)/appendTo.txt | sed s/,\[^,]\*\.cub,/,file.cub,/ \
	  | sed 's/\([0-9][0-9]*\.[0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/output.txt
	rm -f $(OUTPUT)/appendTo.txt
