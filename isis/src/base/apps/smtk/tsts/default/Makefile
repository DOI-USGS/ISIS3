APPNAME = smtk

# 1.52587890625e-05
smtkTruth.cub.TOLERANCE = 100.0

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/0166613_crop_equi.cub \
       match=$(INPUT)/0027399_crop_equi.cub regdef=$(INPUT)/mar10.def \
	   to=$(OUTPUT)/smtkTruth.cub nseed=30 space=3 subcbox=3 seedsample=8.0 \
       plotdist=10 > /dev/null;
