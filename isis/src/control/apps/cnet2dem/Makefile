include $(ISISROOT)/make/isismake.apps

ifeq ($(HOST_ARCH),Darwin)
ALLINCDIRS += -I/work/users/kbecker/NaturalNeighbors/include
ALLLIBDIRS += -L/work/users/kbecker/NaturalNeighbors/lib
else
ALLINCDIRS += -I$(ISISROOT)/3rdParty/extras/include
ALLLIBDIRS += -L$(ISISROOT)/3rdParty/extras/lib
endif

ALLLIBS += -lnn

