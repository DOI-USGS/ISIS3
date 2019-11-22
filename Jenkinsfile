// vim: ft=groovy

pipeline
{
    agent any
    stages {
        stage ("CI") {
            steps {
                script {
                    def groovy_utilities = load "${pwd()}/script.groovy"

                    def isisDataPath = '/isisData/data'

                    def isisMgrScripts = '/isisData/data/isis3mgr_scripts'

                    def isisTestDataPath = "/isisData/testData"

                    def kakaduIncDir = "/isisData/kakadu"

                    def isisEnv = [
                        "ISIS3DATA=${isisDataPath}",
                        "ISIS3TESTDATA=${isisTestDataPath}",
                        "ISIS3MGRSCRIPTS=${isisMgrScripts}"
                    ]

                    def cmakeFlags = [
                        "-DJP2KFLAG=ON",
                        "-DKAKADU_INCLUDE_DIR=${kakaduIncDir}",
                        "-Dpybindings=OFF",
                        "-DCMAKE_BUILD_TYPE=RELEASE"
                    ]

                    def build_ok = true
                    def errors = []

                    def labels = ['CentOS', 'Fedora'] // labels for Jenkins node types we will build on
                    def builders = [:]

                    for (x in labels) {
                        def label = x // Need to bind the label variable before the closure - can't do 'for (label in labels)'
                        def lower_label = x.toLowerCase()

                        // Create a map to pass in to the 'parallel' step so we can fire all the builds at once
                        builders[lower_label] = {
                            node(lower_label) {
                                stage ("${label}") {

                                    env.STAGE_STATUS = "Checking out ISIS"
                                    checkout scm

                                    env.STAGE_STATUS = "Creating conda environment"
                                    sh '''
                                      # Use the conda cache running on the Jenkins host
                                      conda config --set channel_alias http://dmz-jenkins.wr.usgs.gov
                                      conda config --set always_yes True
                                      conda create -n isis python=3
                                    '''

                                    if (label == "centos") {
                                      sh 'conda env update -n isis -f environment_gcc4.yml --prune'
                                    } else {
                                      sh 'conda env update -n isis -f environment.yml --prune'
                                    }

                                    withEnv(isisEnv) {
                                        dir("${env.ISISROOT}") {
                                            try {
                                                env.STAGE_STATUS = "Building ISIS on ${env.OS}"
                                                sh """
                                                    source activate isis
                                                    ls ../
                                                    ls .
                                                    echo `pwd`
                                                    cmake -GNinja ${cmakeFlags.join(' ')} ../isis
                                                    ninja -j4 install
                                                    python ../isis/scripts/isis3VarInit.py --data-dir ${env.ISIS3DATA} --test-dir ${env.ISIS3TESTDATA}
                                                """
                                            }
                                            catch(e) {
                                                build_ok = false
                                                errors.add(env.STAGE_STATUS)
                                                println e.toString()
                                            }
                                        }

                                        if(build_ok) {
                                            currentBuild.result = "SUCCESS"
                                        }
                                        else {
                                            currentBuild.result = "FAILURE"
                                            def comment = "Failed during:\n"
                                            errors.each {
                                                comment += "- ${it}\n"
                                            }
                                            setGitHubBuildStatus(comment)
                                        }
                                    }
                                }
                            }
                        }
                    }
                    parallel builders
                }
            }
        }
    }
}
