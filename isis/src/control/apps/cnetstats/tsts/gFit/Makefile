# @history 2016-09-29 Ian Humphrey - Modified the SED command to leave "input" in the resulting
#                         edit, so we don't need to worry about properly matching comma groups.
APPNAME = cnetstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/cnet.net \
	  filter=yes \
	  deffile=$(INPUT)/deffile.def \
	  flatfile=$(OUTPUT)/out.txt > /dev/null;
	$(SED) 's#/.*/input/#input/#' $(OUTPUT)/out.txt > $(OUTPUT)/truth.txt;
	$(RM) $(OUTPUT)/cube.lis $(OUTPUT)/out.txt $(OUTPUT)/out1.txt;
