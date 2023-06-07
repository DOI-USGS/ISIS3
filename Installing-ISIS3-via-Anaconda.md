These are alpha instructions describing how to install ISIS3 via the cross platform [Anaconda package manager](https://conda.io/docs/). These instructions have been tested in linux environments; we do not yet support OS X using this method. The below has also been tested on windows using the Ubuntu subsystem!

If you test this method and run into any issues, please [open an issue](https://github.com/DOI-USGS/ISIS3/issues/new)

```
#first also need libGL in system
sudo apt-get install libglu1
#install X11
sudo apt install x11-apps

#Now on to ISIS3
conda create -n isis3
source activate isis3

#For anaconda also need opengl and X
conda install -c anaconda pyopengl=3.1.1a1
conda install -c conda-forge xorg-libxi

conda config --add channels usgs-astrogeology
conda config --add channels conda-forge
conda config --add channels krodriguez
conda install -c krodriguez isis3

# Set the ISISROOT env variable
export ISISROOT=$CONDA_PREFIX
# export ISISROOT=$CONDA_PREFIX >> ~/.bashrc && source ~/.bashrc 

# Get the ISIS3 data

```

This is a working, albeit not super useful installation. You will need the ISIS3 data to be able to do any real processing.

```
#need some base data for running anything
#warning - this is a lot of data - the MRO kernels area is pretty large
mkdir ~/isis3data
cd ~/isis3data
rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/base data/
# optional for testing CTX processing below (which doesn't require image kernels)
rsync -azv --delete --partial --exclude='kernels' isisdist.astrogeology.usgs.gov::isis3data/data/mro data/
echo 'export ISIS3DATA=~/isis3data/data' >> ~/.bashrc && source ~/.bashrc
```

Here is an example use case. This is 'regular old ISIS3' from here on out.

```
mkdir ~/isis3_test
cd ~/isis3_test
wget https://raw.githubusercontent.com/DOI-USGS/ISIS3_scripts/master/MRO_CTX_process_all/CTX_process_all.csh
wget -nd http://pdsimage.wr.usgs.gov/Missions/Mars_Reconnaissance_Orbiter/CTX/mrox_0674/data/P22_009816_1745_XI_05S073W.IMG
printf "Group = Mapping\n  ProjectionName  = SimpleCylindrical\n   CenterLongitude = 0.0\n  CenterLatitude  = 0.0\nEnd_Group\nEnd" > simp0.map
csh CTX_process_all.csh simp0.map  0
```