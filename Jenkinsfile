// vim: ft=groovy

def NUM_CORES = 4
def errors = []
def labels = ['Ubuntu'] // labels for Jenkins node types we will build on
def nodes = [:] 

for (lbl in labels) {
    def label = lbl
    def envFile = (label == "CentOS") ? "environment_gcc4.yml" : "environment.yml"

    nodes[label] = {
        stage(label) {
            isisNode(label) {
                def isisEnv = [
                    "ISISDATA=/isisData/isis_data",
                    "ISISTESTDATA=/isisData/isis_testData",
                    "ISIS3MGRSCRIPTS=/isisData/data/isis3mgr_scripts",
                    "MALLOC_CHECK_=1"
                ]

                def cmakeFlags = [
                    "-DJP2KFLAG=ON",
                    "-DKAKADU_INCLUDE_DIR=/isisData/kakadu",
                    "-Dpybindings=OFF",
                    "-DCMAKE_BUILD_TYPE=RELEASE"
                ]

                def stageStatus = "Checking out ISIS on ${label}"

                // Checkout
                checkout scm

                condaEnv("isis3") {
                    // Environment
                    loginShell """
                        conda config --env --set channel_alias https://conda.prod-asc.chs.usgs.gov
                        conda config --env --set remote_read_timeout_secs 3600
                        conda install -c conda-forge python=3 findutils
                        conda env update -f ${envFile} --prune
                        mkdir build install
                    """

                    def osFailed = false
                    isisEnv.add("ISISROOT=${pwd()}/build")
                    isisEnv.add("PATH=${pwd()}/install/bin:${env.PATH}")
                    cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")

                    withEnv(isisEnv) {
                        dir(env.ISISROOT) {
                            // Build
                            stageStatus = "Building ISIS on ${label}"
                            try {
                                loginShell """
                                    cmake -GNinja ${cmakeFlags.join(' ')} ../isis
                                    ninja -j${NUM_CORES} install
                                """
                            } catch(e) {
                                // Die right here
                                error stageStatus
                            }

                            // Unit tests
                            stageStatus = "Running unit tests on ${label}"
                            try {
                                loginShell "ctest -R _unit_ -j${NUM_CORES} --output-on-failure"
                            } catch(e) {
                                errors.add(stageStatus)
                                osFailed = true
                            }

                            // App tests
                            stageStatus = "Running app tests on ${label}"
                            try {
                                loginShell "ctest -R _app_ -j${NUM_CORES} --output-on-failure --timeout 10000"
                            } catch(e) {
                                errors.add(stageStatus)
                                osFailed = true
                            }

                            try {
                                loginShell "ctest -R _module_ -j${NUM_CORES} --output-on-failure --timeout 10000" 
                            } catch(e) {
                                errors.add(stageStatus)
                                osFailed = true
                            }

                            // Gtests
                            stageStatus = "Running gtests on ${label}"
                            try {
                                loginShell "ctest -R '.' -E '(_app_|_unit_|_module_)' -j${NUM_CORES} --output-on-failure --timeout 10000"
                            } catch(e) {
                                errors.add(stageStatus)
                                osFailed = true
                            }

                            if (osFailed) {
                                error "Failed on ${label}"
                            }
                        }
                    }
                }
            }
        }
    }
}

// Run the builds in parallel
node {
    try {
        parallel nodes

    } catch(e) {
        // Report result to GitHub
        currentBuild.result = "FAILURE"
        
        def comment = "Failed during:\n"
        errors.each {
            comment += "- ${it}\n"
        }

        setGithubStatus(comment)
    }
}
