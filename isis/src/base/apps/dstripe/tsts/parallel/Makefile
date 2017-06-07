APPNAME = dstripe

include $(ISISROOT)/make/isismake.tsts

commands:
	cat $(INPUT)/root.lis | xargs -n1 -P 2 -IX $(APPNAME) from=$(INPUT)/X.cub to=$(OUTPUT)/X.dstr.cub mode=vert vlnl=51 vhns=51 > /dev/null;
