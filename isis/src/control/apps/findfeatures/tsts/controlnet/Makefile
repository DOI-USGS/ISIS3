# This tests the NETWORKID, NETTYPE, ONET, POINTID, TOLIST params
APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=surf/surf \
	           onet=$(OUTPUT)/groundMercuryTruth.net \
	           nettype=ground \
	           networkid="NETTYPE ground, TARGET MERCURY" \
	           target="MERCURY" \
	           tolist=$(OUTPUT)/groundMercuryInputs.temp.txt \
	           > /dev/null;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=surf/surf \
	           onet=$(OUTPUT)/groundMercuryMaxpoints.net \
	           nettype=ground \
	           networkid="NETTYPE ground, MAXPOINTS 400" \
	           maxpoints=400 \
	           > /dev/null;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=surf/surf \
	           onet=$(OUTPUT)/customPointsTruth.net \
	           networkid="NETTYPE image, POINTID ff_test_???, POINTINDEX 314" \
	           pointid="ff_test_???" \
	           pointindex=314 \
	           > /dev/null;

	# Need to trim filepaths from TOLIST output
	$(SED) 's|.*/\([^/]*\)|\1|' $(OUTPUT)/groundMercuryInputs.temp.txt \
	       >& $(OUTPUT)/groundMercuryInputs.txt;
	$(RM) $(OUTPUT)/groundMercuryInputs.temp.txt;
