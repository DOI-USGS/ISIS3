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
	$(RM) $(OUTPUT)/input_*
