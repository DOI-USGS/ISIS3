APPNAME = mocevenodd

include $(ISISROOT)/make/isismake.tsts

commands:
	moc2isis \
	  from=$(INPUT)/fha00491.imq \
	  to=$(OUTPUT)/mgsTemp.cub >& /dev/null;
	spiceinit from=$(OUTPUT)/mgsTemp.cub >& /dev/null;
	$(APPNAME) from=$(OUTPUT)/mgsTemp.cub \
	  to=$(OUTPUT)/mocevenoddTruth.cub >& /dev/null;
