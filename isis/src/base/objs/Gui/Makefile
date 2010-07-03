INCS  := $(wildcard *.h)
QOBJ  := $(shell grep -HIE -m1 'Q_OBJECT' $(INCS)) #get all header files with 'Q_OBJECT' Output=%.h:   QOBJECT
QCOLN := $(filter-out Q_OBJECT, $(QOBJ))           #filter out 'Q_OBJECT from the Output
QINC  := $(subst  :, , $(QCOLN))		   #substitute  ':' with ''
CPPS  := $(filter-out moc_%.cpp unitTest.cpp, $(wildcard *.cpp))#no moc+unitTest cpp files	  
SRCS  := $(CPPS) $(QINC:%.h=moc_%.cpp) 
OBJS  := $(SRCS:%.cpp=%.o) 

include $(ISISROOT)/make/isismake.objs
