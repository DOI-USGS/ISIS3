APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/testpvl*.pvl >& $(OUTPUT)/pvlreader_data.lis;
	$(APPNAME) config=$(INPUT)/pvlreader_test.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" > /dev/null;
	$(RM) $(OUTPUT)/pvlreader_data.lis;

