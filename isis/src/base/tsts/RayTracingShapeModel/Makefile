INGEST = msi2isis
SPICEINIT = spiceinit
PROJECT = cam2map
COPY = /bin/cp
REMOVE = /bin/rm

IMAGE = m0132383354f4_2p_iof_dbl
DSK = ErosHR.bds
MAP = simp_planetocentric_50k.map

$(IMAGE)_Bullet_SC.cub.TOLERANCE = 0.00001
$(IMAGE)_Standard_SC.cub.TOLERANCE = 0.00001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(INGEST) $(TSTARGS) from=$(INPUT)/$(IMAGE).lbl to=$(OUTPUT)/$(IMAGE).cub > /dev/null;
	$(INGEST) $(TSTARGS) from=$(INPUT)/$(IMAGE).lbl to=$(OUTPUT)/$(IMAGE).cub > /dev/null;
	$(COPY) $(OUTPUT)/$(IMAGE).cub $(OUTPUT)/$(IMAGE)_Bullet.cub;
	$(COPY) $(OUTPUT)/$(IMAGE).cub $(OUTPUT)/$(IMAGE)_Embree.cub;
	$(COPY) $(OUTPUT)/$(IMAGE).cub $(OUTPUT)/$(IMAGE)_Standard.cub;
	$(SPICEINIT) from=$(OUTPUT)/$(IMAGE)_Bullet.cub -pref=$(INPUT)/MyPreferences_Bullet \
	             shape=user model=$(INPUT)/$(DSK) > /dev/null;
	$(SPICEINIT) from=$(OUTPUT)/$(IMAGE)_Embree.cub -pref=$(INPUT)/MyPreferences_Embree \
	             shape=user model=$(INPUT)/$(DSK) > /dev/null;
	$(SPICEINIT) from=$(OUTPUT)/$(IMAGE)_Standard.cub -pref=$(INPUT)/MyPreferences_Standard \
	             shape=user model=$(INPUT)/$(DSK) > /dev/null;
	$(PROJECT) from=$(OUTPUT)/$(IMAGE)_Bullet.cub -pref=$(INPUT)/MyPreferences_Bullet \
	           to=$(OUTPUT)/$(IMAGE)_Bullet_SC.cub map=$(INPUT)/$(MAP) pixres=map \
	           lonseam=auto > /dev/null;
# Do not project Embree because it has trouble with the Eros DSK and does not
# reproduce itself well. JAM
# 	$(PROJECT) from=$(OUTPUT)/$(IMAGE)_Embree.cub -pref=$(INPUT)/MyPreferences_Embree \
# 	           to=$(OUTPUT)/$(IMAGE)_Embree_SC.cub map=$(INPUT)/$(MAP) pixres=map \
# 	           lonseam=auto > /dev/null;
	$(PROJECT) from=$(OUTPUT)/$(IMAGE)_Standard.cub -pref=$(INPUT)/MyPreferences_Standard \
	           to=$(OUTPUT)/$(IMAGE)_Standard_SC.cub map=$(INPUT)/$(MAP) pixres=map \
	           lonseam=auto > /dev/null;
	$(REMOVE) $(OUTPUT)/$(IMAGE).cub;
	$(REMOVE) $(OUTPUT)/$(IMAGE)_Bullet.cub;
	$(REMOVE) $(OUTPUT)/$(IMAGE)_Embree.cub;
	$(REMOVE) $(OUTPUT)/$(IMAGE)_Standard.cub;
