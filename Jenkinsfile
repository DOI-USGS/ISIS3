node("centos && isis") {

    def build_ok = true

    stage("Report") {
        def container_os = sh(script: '''grep -Po '(?<=PRETTY_NAME=").*(?=")' /etc/os-release''', returnStdout: true).trim()
        def container_mounts = sh(script: 'df -h', returnStdout: true).trim()
        println "Container OS: ${container_os}"
        println "Container Mounts:"
        println "${container_mounts}"
    }
    stage("Checkout") {
        checkout scm
    }
    stage("SetupEnvironment"){
        dir("ISIS3") {
            sh "ls -l"
            sh "git submodule update --recursive --init"
            sh "conda config --set ssl_verify false"
            // sh "conda config --set channel_alias https://astro-bin.wr.usgs.gov/artifactory/conda"
            // sh "conda config --set ssl_verify false"
            sh "conda env create -n testEnvCentos -f environment_gcc4.yml"
            sh "source activate testEnvCentos"
            sh "conda install -c conda-forge vim"
            sh "conda install -c conda-forge findutils"
        }
    }
    stage("Build") {
        dir("ISIS3") {
            sh "mkdir build install"
              dir("build") {
                sh """
                    source activate testEnvCentos
                    conda update -n base -c defaults conda
                    cmake -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData -DJP2KFLAG=ON -Dpybindings=OFF -DCMAKE_BUILD_TYPE=RELEASE -GNinja ../isis
                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                    ninja -j4 install
                """
            }
        }
    }
    try{
        stage("UnitTests") {
            dir("ISIS3") {
                dir("build") {
                    sh """
                        source activate testEnvCentos
                        source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                        ctest -R _unit_ -j4 -VV
                    """
                }
            }
        }
    }
    catch(e) {
        build_ok = false
        echo e.toString()
    }
    try{
        stage("AppTests") {
            dir("ISIS3") {
                dir("build") {
                    sh """
                        source activate testEnvCentos
                        source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                        ctest -R _app_ -j4 -VV
                    """
                }
            }
        }
    }
    catch(e) {
        build_ok = false
        echo e.toString()
    }
    try{
        stage("ModuleTests") {
            dir("ISIS3") {
                dir("build") {
                    sh """
                        source activate testEnvCentos
                        source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                        ctest -j4 -VV -R _module_
                    """
                }
            }
        }
    }
    catch(e) {
        build_ok = false
        echo e.toString()
    }
    try{
        stage("GTests") {
            dir("ISIS3") {
                dir("build") {
                    sh """
                        source activate testEnvCentos
                        source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                        ctest -j4 -VV -R "." -E "(_app_|_unit_|_module_)"
                    """
                }
            }
        }
    }
    catch(e) {
        build_ok = false
        echo e.toString()
    }

    if(build_ok) {
        currentBuild.result = "SUCCESS"
    }
    else {
        currentBuild.result = "FAILURE"
    }

}
