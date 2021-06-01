equalizerTruth4.cub.TOLERANCE = .00005
equalizerTruth5.cub.TOLERANCE = .00007

APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/I0*.cub > $(OUTPUT)/FromList.lst;
	$(ECHO) $(INPUT)/I01523019RDR.lev2.cub > $(OUTPUT)/HoldList.lst;
	$(APPNAME) fromlist=$(OUTPUT)/FromList.lst \
	  holdlist=$(OUTPUT)/HoldList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  solvemethod=qrd adjust=gain \
	  > /dev/null;
	$(MV) I00824006RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth1.cub;
	$(MV) I01523019RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth2.cub;
	$(MV) I02609002RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth3.cub;
	$(LS) $(INPUT)/EN*.cub > $(OUTPUT)/files.lis;
	$(LS) $(INPUT)/EN*top*.cub > $(OUTPUT)/hold.lis;
	$(ECHO) "$(OUTPUT)/equalizerTruth4.cub" > $(OUTPUT)/tolist.lis
	$(ECHO) "$(OUTPUT)/equalizerTruth5.cub" >> $(OUTPUT)/tolist.lis
	$(ECHO) "$(OUTPUT)/equalizerTruth6.cub" >> $(OUTPUT)/tolist.lis
	$(APPNAME) fromlist=$(OUTPUT)/files.lis holdlist=$(OUTPUT)/hold.lis \
	  adjust=gain tolist=$(OUTPUT)/tolist.lis > /dev/null;
	$(RM) $(OUTPUT)/files.lis $(OUTPUT)/hold.lis $(OUTPUT)/tolist.lis;
	$(RM) $(OUTPUT)/FromList.lst;
	$(RM) $(OUTPUT)/HoldList.lst;

