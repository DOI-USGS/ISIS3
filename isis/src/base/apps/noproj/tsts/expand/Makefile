APPNAME = noproj
#This test exercises the expand options

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/f387a06.findrx.cub \
	to=$(OUTPUT)/vik.noproj.expanded.cub \
	sampexp=10 lineexp=5 > /dev/null;
	$(RM) print.prt > /dev/null;
