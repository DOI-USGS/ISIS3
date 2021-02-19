APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -r $(INPUT)/*.cub > $(OUTPUT)/FromList.lst;
	$(CAT) $(INPUT)/instats.pvl | $(SED) "s|FileName = .*odyssey/testData/|FileName = $(INPUT)/|" > $(OUTPUT)/instats.pvl;
	$(APPNAME) fromlist=$(OUTPUT)/FromList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  instats=$(OUTPUT)/instats.pvl \
	  process=apply \
	  > /dev/null;
	$(MV) I00824006RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth1.cub;
	$(MV) I01523019RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth2.cub;
	$(RM) $(OUTPUT)/FromList.lst;
	$(RM) $(OUTPUT)/instats.pvl;

