APPNAME = hijitreg

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_001446_1790_RED2.balance.cub \
	           match=$(INPUT)/PSP_001446_1790_RED3.balance.cub \
	           flatfile=$(OUTPUT)/PSP_001446_1790_RED.flat.txt \
	           cnetfile=$(OUTPUT)/PSP_001446_1790_RED.net.txt > /dev/null
