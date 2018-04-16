APPNAME = mdis2isis

#NOPREFERENCES=
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0001426030M.IMG \
	  to=$(OUTPUT)/mdis2isisTruth.cub > /dev/null
	catlab from=$(OUTPUT)/mdis2isisTruth.cub \
	  to=$(OUTPUT)/mdis2isisTruth.pvl> /dev/null
	$(APPNAME) from=$(INPUT)/EN0089569526M.IMG \
	  to=$(OUTPUT)/mdis2isisLutTruth.cub > /dev/null
	catlab from=$(OUTPUT)/mdis2isisLutTruth.cub \
	  to=$(OUTPUT)/mdis2isisLutTruth.pvl> /dev/null
	$(APPNAME) from=$(INPUT)/EN0131771763M.IMG \
	  to=$(OUTPUT)/mdis2isisStrangeLabelTruth.cub > /dev/null
	catlab from=$(OUTPUT)/mdis2isisStrangeLabelTruth.cub \
	  to=$(OUTPUT)/mdis2isisStrangeLabelTruth.pvl> /dev/null
