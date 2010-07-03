APPNAME = histitch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from1=$(INPUT)/Channel0.cub \
	  from2=$(INPUT)/Channel1.cub \
	  to=$(OUTPUT)/histitch_RED_balanced_mult.cub \
	  balance=equalize skip=0 seamsize=3 channel=0 \
	  width=201 fill=true operator=multiply > /dev/null;
