INPUTFILE = 3800r
IMPORT = gllssi2isis
CALIBRATE = gllssical

include $(ISISROOT)/make/isismake.tsts

commands:
	$(IMPORT) \
	  from=$(INPUT)/$(INPUTFILE).lbl \
	  to=$(OUTPUT)/$(INPUTFILE).cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/$(INPUTFILE).cub > $(OUTPUT)/importLabel.pvl;
	$(CALIBRATE) \
	  from=$(OUTPUT)/$(INPUTFILE).cub \
	  to=$(OUTPUT)/$(INPUTFILE).rad.cub \
	  bitweighting=true \
	  scale=0.0001 \
	  units=radiance \
	  > /dev/null;
