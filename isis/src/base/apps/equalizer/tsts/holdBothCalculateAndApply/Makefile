# This test passes in a holdlist, lets process default to both (i.e. calculate
# statistics and apply them), and lets adjust default to both (contrast and
# brightness).
#
# It is run once without creating the output statistics file and a second time
# creating the outstats file for test coverage.
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/I0*.cub > $(OUTPUT)/FromList.lst;
	$(ECHO) $(INPUT)/I01523019RDR.lev2.cub > $(OUTPUT)/HoldList.lst;
	$(APPNAME) fromlist=$(OUTPUT)/FromList.lst \
	  holdlist=$(OUTPUT)/HoldList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  solvemethod=qrd \
	  > /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/FromList.lst \
	  holdlist=$(OUTPUT)/HoldList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  outstats=$(OUTPUT)/outstatstmp.pvl \
	  solvemethod=qrd \
	  > /dev/null;
	$(CAT) $(OUTPUT)/outstatstmp.pvl | $(SED) "s|FileName = .*/I0|FileName = I0|" > $(OUTPUT)/outstats.pvl;
	$(RM) $(OUTPUT)/outstatstmp.pvl;
	$(MV) I00824006RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth1.cub;
	$(MV) I01523019RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth2.cub;
	$(MV) I02609002RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth3.cub;
	$(RM) $(OUTPUT)/FromList.lst;
	$(RM) $(OUTPUT)/HoldList.lst;

