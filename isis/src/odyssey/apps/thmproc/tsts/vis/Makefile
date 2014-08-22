# Test for VIS images
#
# The first run is a test for no ingestion
#
# The second run is a test for ingesting EDR with mapping,  no viscleanup
#
# The third run is a test for ingesting EDR with no mapping, viscleanup
#
# The fourth run is a test for ingesting RDR with mapping, viscleanup
#
APPNAME = thmproc

V05642009RDR.cub.TOLERANCE = 0.0000000002

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/V14093008RDR.even.cub $(OUTPUT)/input_V14093008RDR.even.cub;
	$(APPNAME) FROM=$(OUTPUT)/input_V14093008RDR.even.cub \
	TO=$(OUTPUT)/V14093008RDR.even.cub \
	INGESTION=no > /dev/null;
	$(CP) $(INPUT)/V01001002EDR.QUB $(OUTPUT)/input_V01001002EDR.QUB;
	$(APPNAME) FROM=$(OUTPUT)/input_V01001002EDR.QUB \
	TO=$(OUTPUT)/V01001002EDR \
	MAPPING=yes \
	VISCLEANUP=no > /dev/null;
	$(CP) $(INPUT)/V05642009EDR.QUB $(OUTPUT)/input_V05642009EDR.QUB;
	$(APPNAME) FROM=$(OUTPUT)/input_V05642009EDR.QUB \
	TO=$(OUTPUT)/V05642009EDR \
	MAPPING=no BANDS=2 > /dev/null;
	$(CP) $(INPUT)/V05642009RDR.QUB $(OUTPUT)/input_V05642009RDR.QUB;
	$(APPNAME) FROM=$(OUTPUT)/input_V05642009RDR.QUB \
	TO=$(OUTPUT)/V05642009RDR \
	MAPPING=yes BANDS=2 PIXRES=200 > /dev/null;
	$(RM) $(OUTPUT)/input_*
