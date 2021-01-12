APPNAME = grid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/world.cub \
	           TO=$(OUTPUT)/truth1.cub \
	           LATINC=45 \
			   LONINC=45 \
			   MODE=GROUND \
			   > /dev/null;
	$(APPNAME) FROM=$(INPUT)/world.cub \
	           TO=$(OUTPUT)/truth2.cub \
	           LATINC=45 \
			   LONINC=45 \
			   MODE=GROUND \
			   ticks=true \
			   > /dev/null;
	$(APPNAME) FROM=$(INPUT)/world.cub \
	           TO=$(OUTPUT)/truth3.cub \
	           LATINC=45 \
			   LONINC=45 \
			   MODE=GROUND \
			   ticks=true \
			   diagonal=true \
			   > /dev/null;
