APPNAME = fx

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/* > $(OUTPUT)/list.txt
	$(APPNAME) fromlist=$(OUTPUT)/list.txt \
	mode=list \
	equation=f1+f2+f3 \
	to=$(OUTPUT)/LISTresult.cub > /dev/null;
	rm $(OUTPUT)/list.txt;
