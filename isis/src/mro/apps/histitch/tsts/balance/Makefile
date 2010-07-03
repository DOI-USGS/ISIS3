APPNAME = histitch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from1=$(INPUT)/Channel0.cub \
	  from2=$(INPUT)/Channel1.cub \
	  to=$(OUTPUT)/histitch_RED_balanced.cub \
          channel=0 balance=true skip=7 seamsize=11 > /dev/null;
