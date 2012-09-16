APPNAME = vims2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/fileList.txt;
	$(APPNAME) fromlist= $(OUTPUT)/fileList.txt \
        to=$(OUTPUT)/vims2mapTruth.cub > /dev/null;
	$(RM) $(OUTPUT)/fileList.txt;
