APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/output.txt $(OUTPUT)/camptTruth1.pvl
	$(APPNAME) \
	from=$(INPUT)/ab102401.cub \
	to=$(OUTPUT)/camptTruth1.pvl \
	> /dev/null 
