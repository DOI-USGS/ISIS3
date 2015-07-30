# This test will print errors thrown by the application.
.IGNORE:
APPNAME = thmproc

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test: choose ingestion or mapping" > $(OUTPUT)/error.txt;
	$(CP) $(INPUT)/V01001002EDR.QUB $(OUTPUT)/input_V01001002EDR.QUB; \
	$(APPNAME) FROM=$(OUTPUT)/input_V01001002EDR.QUB \
	TO=$(OUTPUT)/V01001002EDR \
	INGESTION=no \
	MAPPING=no 2>> $(OUTPUT)/error.txt > /dev/null;
	$(RM) $(OUTPUT)/input_* ;
	echo -e "Error Test: Nonexistent PCK file" >> $(OUTPUT)/error.txt;
	$(CP) $(INPUT)/V01001002EDR.QUB $(OUTPUT)/input_V01001002EDR.QUB; \
	$(APPNAME) FROM=$(OUTPUT)/input_V01001002EDR.QUB \
	TO=$(OUTPUT)/V01001002EDR \
	INGESTION=yes \
	MAPPING=no \
	PCK=dummy.pck 2>> $(OUTPUT)/error.txt > /dev/null;
	$(RM) $(OUTPUT)/input_*
	echo -e "Error Test: Nonexistent CK file" >> $(OUTPUT)/error.txt;
	$(CP) $(INPUT)/V01001002EDR.QUB $(OUTPUT)/input_V01001002EDR.QUB; \
        $(APPNAME) FROM=$(OUTPUT)/input_V01001002EDR.QUB \
        TO=$(OUTPUT)/V01001002EDR \
        INGESTION=yes \
        MAPPING=no \
        CK=dummy.ck 2>> $(OUTPUT)/error.txt > /dev/null;
	$(RM) $(OUTPUT)/input_*
	echo -e "Error Test: Nonexistent SPK file" >> $(OUTPUT)/error.txt;
	$(CP) $(INPUT)/V01001002EDR.QUB $(OUTPUT)/input_V01001002EDR.QUB; \
        $(APPNAME) FROM=$(OUTPUT)/input_V01001002EDR.QUB \
        TO=$(OUTPUT)/V01001002EDR \
        INGESTION=yes \
        MAPPING=no \
        SPK=dummy.spk 2>> $(OUTPUT)/error.txt > /dev/null;
	$(RM) $(OUTPUT)/input_*
	echo -e "Error Test: Nonexistent MAP file" >> $(OUTPUT)/error.txt;
	$(CP) $(INPUT)/V01001002EDR.QUB $(OUTPUT)/input_V01001002EDR.QUB; \
        $(APPNAME) FROM=$(OUTPUT)/input_V01001002EDR.QUB \
        TO=$(OUTPUT)/V01001002EDR \
        INGESTION=yes \
        MAPPING=yes \
        MAP=dummy.ck 2>> $(OUTPUT)/error.txt > /dev/null;
	$(RM) $(OUTPUT)/input_*
	echo -e "Error Test: Nonexistent MODEL file" >> $(OUTPUT)/error.txt;
	$(CP) $(INPUT)/V01001002EDR.QUB $(OUTPUT)/input_V01001002EDR.QUB; \
        $(APPNAME) FROM=$(OUTPUT)/input_V01001002EDR.QUB \
        TO=$(OUTPUT)/V01001002EDR \
        INGESTION=yes \
        MAPPING=no \
        SHAPE=USER \
        MODEL=dummy.ck 2>> $(OUTPUT)/error.txt > /dev/null;
	$(RM) $(OUTPUT)/input_*
