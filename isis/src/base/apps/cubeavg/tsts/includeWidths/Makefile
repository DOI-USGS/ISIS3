APPNAME = cubeavg

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/hirdrgen_color_test.cub \
	TO=$(OUTPUT)/delete.cub \
	> /dev/null;
	$(APPNAME) FROM=$(INPUT)/hirdrgen_color_test.cub \
        TO=$(OUTPUT)/compute.cub \
        BANDBIN=compute \
        > /dev/null;
	$(APPNAME) FROM=$(INPUT)/hirdrgen_color_test.cub \
        TO=$(OUTPUT)/user.cub \
        BANDBIN=user \
	CENTER=650 WIDTH=150 \
        > /dev/null;
