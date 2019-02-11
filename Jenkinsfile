properties([pipelineTriggers([githubPush()])])

def nodes = [:]

nodes["centos-7"] = {
    node {
        docker.image("usgsastro/jenkins-worker-isisdeps:centos-7").inside("-v /usgs:/usgs -v /scratch:/scratch") {
            withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
                stage ("CentOS 7") {
                    git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
    //                sh """
    //                    git clone https://github.com/abseil/googletest.git gtest
    //                    conda config --set channel_alias http://astro-bin.wr.usgs.gov/conda
    //                    conda config --prepend channels anaconda
    //                    conda config --append channels conda-forge
    //                    conda config --append channels usgs-astrogeology
    //                    conda config --prepend default_channels anaconda
    //                    conda env create -n isis3 -f environment.yml
    //                    source activate isis3
    //                    mkdir -p ./install ./build && cd build
    //                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
    //                    set +e
    //                    ninja -j8 && ninja install
    //                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
    //                    ctest -V -R _unit_ --timeout 500
    //                    ctest -V -R _app_ --timeout 500
    //                    ctest -V -R _module_ --timeout 500
    //                """
                }
            }
        }
    }
}

nodes["debian-9"] = {
    node {
        docker.image("usgsastro/jenkins-worker-isisdeps:debian-9").inside("-v /usgs:/usgs -v /scratch:/scratch") {
            withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
                stage ("Debian 9") {
                    git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
    //                sh """
    //                    git clone https://github.com/abseil/googletest.git gtest
    //                    conda config --set channel_alias http://astro-bin.wr.usgs.gov/conda
    //                    conda config --prepend channels anaconda
    //                    conda config --append channels conda-forge
    //                    conda config --append channels usgs-astrogeology
    //                    conda config --prepend default_channels anaconda
    //                    conda env create -n isis3 -f environment.yml
    //                    source activate isis3
    //                    mkdir -p ./install ./build && cd build
    //                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
    //                    set +e
    //                    ninja -j8 && ninja install
    //                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
    //                    ctest -V -R _unit_ --timeout 500
    //                    ctest -V -R _app_ --timeout 500
    //                    ctest -V -R _module_ --timeout 500
    //                """
                }
            }
        }
    }
}

nodes["fedora-25"] = {
    node {
        docker.image("usgsastro/jenkins-worker-isisdeps:fedora-25").inside("-v /usgs:/usgs -v /scratch:/scratch") {
            withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
                stage ("Fedora 25") {
                    git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
    //                sh """
    //                    git clone https://github.com/abseil/googletest.git gtest
    //                    conda config --set channel_alias http://astro-bin.wr.usgs.gov/conda
    //                    conda config --prepend channels anaconda
    //                    conda config --append channels conda-forge
    //                    conda config --append channels usgs-astrogeology
    //                    conda config --prepend default_channels anaconda
    //                    conda env create -n isis3 -f environment.yml
    //                    source activate isis3
    //                    mkdir -p ./install ./build && cd build
    //                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
    //                    set +e
    //                    ninja -j8 && ninja install
    //                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
    //                    ctest -V -R _unit_ --timeout 500
    //                    ctest -V -R _app_ --timeout 500
    //                    ctest -V -R _module_ --timeout 500
    //                """
                }
            }
        }
    }
}

nodes["ubuntu-1804"] = {
    node {
        docker.image("usgsastro/jenkins-worker-isisdeps:ubuntu-1804").inside("-v /usgs:/usgs -v /scratch:/scratch") {
            withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
                stage ("Ubuntu 18.04") {
                    git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
    //                sh """
    //                    git clone https://github.com/abseil/googletest.git gtest
    //                    conda config --set channel_alias http://astro-bin.wr.usgs.gov/conda
    //                    conda config --prepend channels anaconda
    //                    conda config --append channels conda-forge
    //                    conda config --append channels usgs-astrogeology
    //                    conda config --prepend default_channels anaconda
    //                    conda env create -n isis3 -f environment.yml
    //                    source activate isis3
    //                    mkdir -p ./install ./build && cd build
    //                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
    //                    set +e
    //                    ninja -j8 && ninja install
    //                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
    //                    ctest -V -R _unit_ --timeout 500
    //                    ctest -V -R _app_ --timeout 500
    //                    ctest -V -R _module_ --timeout 500
    //                """
                }
            }
        }
    }
}

nodes["mac1013"] = {
    node("mac1013") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("MacOS 10.13 (High Sierra)") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
//                sh """
//                    git clone https://github.com/abseil/googletest.git gtest
//                    conda config --set channel_alias http://astro-bin.wr.usgs.gov/conda
//                    conda config --prepend channels anaconda
//                    conda config --append channels conda-forge
//                    conda config --append channels usgs-astrogeology
//                    conda config --prepend default_channels anaconda
//                    conda env create -n isis3 -f environment.yml
//                    source activate isis3
//                    mkdir -p ./install ./build && cd build
//                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
//                    set +e
//                    ninja -j8 && ninja install
//                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
//                    ctest -V -R _unit_ --timeout 500
//                    ctest -V -R _app_ --timeout 500
//                    ctest -V -R _module_ --timeout 500
//                """
            }
        }
    }
}

parallel nodes
