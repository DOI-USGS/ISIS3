# test setting either SAMPLE or LINE, not both together
APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	to=$(OUTPUT)/onlySampleTruth.pvl \
	sample=10 > /dev/null;
	cat $(OUTPUT)/onlySampleTruth.pvl | \
	$(SED) 's/\([0-9]*\.[0-9]\{5\}\)[0-9]*/\1/g' | \
	$(SED) '/Filename.*/ { N; N; N; N; s/-\n[ ]*//g; }' | \
	$(SED) 's/\(.*= \).*\(ab102401.cub\)/\1\2/' \
	> $(OUTPUT)/onlySampleTruthtmp.pvl;
	$(MV) $(OUTPUT)/onlySampleTruthtmp.pvl \
	$(OUTPUT)/onlySampleTruth.pvl;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	to=$(OUTPUT)/onlyLineTruth.pvl \
	line=10 > /dev/null;
	cat $(OUTPUT)/onlyLineTruth.pvl | \
	$(SED) 's/\([0-9]*\.[0-9]\{5\}\)[0-9]*/\1/g' | \
	$(SED) '/Filename.*/ { N; N; N; N; s/-\n[ ]*//g; }' | \
	$(SED) 's/\(.*= \).*\(ab102401.cub\)/\1\2/' \
	> $(OUTPUT)/onlyLineTruthtmp.pvl;
	$(MV) $(OUTPUT)/onlyLineTruthtmp.pvl \
	$(OUTPUT)/onlyLineTruth.pvl;
