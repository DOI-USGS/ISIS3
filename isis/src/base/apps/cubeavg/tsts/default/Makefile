APPNAME = cubeavg

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/isis3vis.cub \
	TO=$(OUTPUT)/delete.cub \
        > /dev/null;
	$(APPNAME) FROM=$(INPUT)/isis3vis.cub \
        TO=$(OUTPUT)/compute.cub \
        BANDBIN=compute \
        > /dev/null;
	$(APPNAME) FROM=$(INPUT)/isis3vis.cub \
        TO=$(OUTPUT)/user.cub \
        BANDBIN=user \
	CENTER=7.0 WIDTH=0.01 \
        > /dev/null;
