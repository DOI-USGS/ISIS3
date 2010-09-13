APPNAME = cnetstats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	  cnet=$(INPUT)/cnet.net \
	  filter=yes \
	  deffile=$(INPUT)/deffile.def \
	  flatfile=$(OUTPUT)/truth.txt \
	  type=csv > /dev/null;
	  $(RM) $(OUTPUT)/cube.lis;
