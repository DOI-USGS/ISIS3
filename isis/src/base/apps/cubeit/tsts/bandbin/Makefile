APPNAME = cubeit

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/inputBandbinList.lis; \
	$(APPNAME) fromlist=$(OUTPUT)/inputBandbinList.lis \
	to=$(OUTPUT)/cubeitBandBinTruth.cub > /dev/null;  \
	catlab from=$(OUTPUT)/cubeitBandBinTruth.cub      \
	to=$(OUTPUT)/labels.pvl > /dev/null; 
	$(RM) $(OUTPUT)/inputBandbinList.lis; 		\

