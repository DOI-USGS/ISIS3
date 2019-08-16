node {
    withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
        stage ("Checkout") {
            checkout scm
            sh 'git clone https://github.com/abseil/googletest.git gtest'
        }

        stage ("Build") {
            conda config --prepend channels anaconda
            conda config --append channels conda-forge
            conda config --append channels usgs-astrogeology
            conda config --prepend default_channels anaconda
            conda env create -n isis3 -f environment.yml
            source activate isis3
            mkdir -p ./install ./build && cd build
            cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
       }
               
       stage("Test") {
            set +e
            ninja -j8 && ninja install
            source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
            ctest -V -R _unit_ --timeout 500
            ctest -V -R _app_ --timeout 500
            ctest -V -R _module_ --timeout 500
        }
    }
}
