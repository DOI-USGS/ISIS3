APPNAME = mdiscal

darkanalysis.cub.TOLERANCE = 0.000001
darkanalysis.pvl.IGNORELINES = ProcessDate Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/EN0210557780M.cub \
	FLATFIELD=false RADIOMETRIC=false \
	TO=$(OUTPUT)/darkanalysis.cub \
	> /dev/null;

	catlab FROM=$(OUTPUT)/darkanalysis.cub \
	   | sed 's/\([0-9][0-9]*\.[0-9]\{10\}\)\([0-9][0-9]*\)/\1/g' \
	   > $(OUTPUT)/darkanalysis.pvl
