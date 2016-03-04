APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/assetsidebar_assetdata.csv . > /dev/null; \
	$(APPNAME) config=$(INPUT)/assetsidebar_test.conf \
	parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" > /dev/null; \
	$(RM) assetsidebar_assetdata.csv > /dev/null; \
	$(MV) assetsidebar_test_createdasset.csv $(OUTPUT); \
	$(MV) assetsidebar_test_nonexistingasset.csv $(OUTPUT) > /dev/null;

