APPNAME = noisefilter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/nsFilTruth1.cub \
	  toldef= dn \
	  tolmin= 100 \
	  tolmax= 110 \
	  replace= average \
	  samples= 21 \
	  lines= 21 \
	  nullisnoise= true \
	  hisisnoise= true \
	  hrsisnoise= true \
	  lisisnoise= true \
	  lrsisnoise= true > /dev/null;
