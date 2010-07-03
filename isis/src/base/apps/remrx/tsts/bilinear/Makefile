APPNAME = remrx

# 2.0
remrxTruth-bilinear.cub.TOLERANCE = 2.01

include $(ISISROOT)/make/isismake.tsts

commands:
	crop from=$(INPUT)/f387a06.cub to=$(OUTPUT)/f387a06.crop.cub > /dev/null;
	editlab from=$(OUTPUT)/f387a06.crop.cub option=modkey grpname=Reseaus keyword=Status value=Calculated > /dev/null;
	$(APPNAME) from=$(OUTPUT)/f387a06.crop.cub to=$(OUTPUT)/remrxTruth-bilinear.cub action=bilinear > /dev/null;
