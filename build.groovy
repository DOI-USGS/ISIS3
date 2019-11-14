// vim: ft=groovy

def isisDataPath = '/isisData/data'

def isisMgrScripts = '/isisData/data/isis3mgr_scripts'

def isisTestDataPath = "/isisData/testData"

def isisEnv = [
    "ISIS3DATA=${isisDataPath}",
    "ISIS3TESTDATA=${isisTestDataPath}",
    "ISIS3MGRSCRIPTS=${isisMgrScripts}"
]

def cmakeFlags = [
    "-DJP2KFLAG=OFF",
    "-Dpybindings=OFF",
    "-DCMAKE_BUILD_TYPE=RELEASE"
]

def build_ok = true
def errors = []

// Helpers for setting commit status
def getRepoUrl() {
    return sh(script: "git config --get remote.origin.url", returnStdout: true).trim()
}

def getCommitSha() {
    return sh(script: "git rev-parse HEAD", returnStdout: true).trim()
}

def setGitHubBuildStatus(status) {
    def repoUrl = getRepoUrl()
    def commitSha = getCommitSha()

    step([
        $class: 'GitHubCommitStatusSetter',
        reposSource: [$class: "ManuallyEnteredRepositorySource", url: repoUrl],
        commitShaSource: [$class: "ManuallyEnteredShaSource", sha: commitSha],
        errorHandlers: [[$class: 'ShallowAnyErrorHandler']],
        statusResultSource: [
          $class: 'ConditionalStatusResultSource',
          results: [
            [$class: 'BetterThanOrEqualBuildResult', result: 'SUCCESS', state: 'SUCCESS', message: status],
            [$class: 'BetterThanOrEqualBuildResult', result: 'FAILURE', state: 'FAILURE', message: status],
            [$class: 'AnyBuildResult', state: 'FAILURE', message: 'Loophole']
          ]
        ]
    ])
}

node("${env.OS.toLowerCase()}") {
    stage ("Checkout") {
        env.STAGE_STATUS = "Checking out ISIS"
        checkout scm
        isisEnv.add("ISISROOT=${pwd()}/build")
        cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")
    }

    stage("Create environment") {
        env.STAGE_STATUS = "Creating conda environment"
        sh '''
            # Use the conda cache running on the Jenkins host
            conda config --set channel_alias http://dmz-jenkins.wr.usgs.gov
            conda config --set always_yes True
            conda create -n isis python=3
        '''

        if (env.OS.toLowerCase() == "centos") {
            sh 'conda env update -n isis -f environment_gcc4.yml --prune'
        } else {
            sh 'conda env update -n isis -f environment.yml --prune'
        }
    }

    withEnv(isisEnv) {
        dir("${env.ISISROOT}") {
            try {
                stage ("Build") {
                    env.STAGE_STATUS = "Building ISIS on ${env.OS}"
                    sh """
                        source activate isis
                        cmake -GNinja ${cmakeFlags.join(' ')} ../isis
                        ninja -j4 install
                        python ../isis/scripts/isis3VarInit.py --data-dir ${env.ISIS3DATA} --test-dir ${env.ISIS3TESTDATA}
                    """
                }
            }
            catch(e) {
                build_ok = false
                errors.add(env.STAGE_STATUS)
                println e.toString()
            }

            if (build_ok) {

                try{
                    stage("UnitTests") {
                        dir("${env.ISISROOT}") {
                            env.STAGE_STATUS = "Running unit tests on ${env.OS}"
                                sh """
                                    source activate isis
                                    echo $ISIS3TESTDATA
                                    echo $ISIS3DATA
                                    echo $PATH

                                    # environment variables
                                    export ISISROOT=${env.ISISROOT}
                                    export ISIS3TESTDATA="/isisData/testData"
                                    export ISIS3DATA='/isisData/data'
                                    export PATH=`pwd`/../../install/bin:/home/jenkins/.conda/envs/isis/bin:$PATH

                                    tabledump -HELP

                                    ctest -R _unit_ -j4 -VV
                                    source deactivate
                                """

                        }
                    }
                }
                catch(e) {
                    build_ok = false
                    echo e.toString()
                }

                try{
                    stage("AppTests") {
                        env.STAGE_STATUS = "Running app tests on ${env.OS}"
                        sh """
                            source activate isis
                            echo $ISIS3TESTDATA
                            echo $ISIS3DATA
                            echo $PATH

                            # environment variables
                            export ISISROOT=${env.ISISROOT}
                            export ISIS3TESTDATA="/isisData/testData"
                            export ISIS3DATA='/isisData/data'
                            export PATH=`pwd`/../../install/bin:/home/jenkins/.conda/envs/isis/bin:$PATH

                            tabledump -HELP

                            ctest -R _app_ -j4 -VV
                            source deactivate

                        """
                    }
                }
                catch(e) {
                    build_ok = false
                    errors.add(env.STAGE_STATUS)
                    println e.toString()
                }

                try{
                    stage("ModuleTests") {
                        env.STAGE_STATUS = "Running module tests on ${env.OS}"
                        sh """
                            source activate isis
                            echo $ISIS3TESTDATA
                            echo $ISIS3DATA
                            echo $PATH

                            # environment variables
                            export ISISROOT=${env.ISISROOT}
                            export ISIS3TESTDATA="/isisData/testData"
                            export ISIS3DATA='/isisData/data'
                            export PATH=`pwd`/../../install/bin:/home/jenkins/.conda/envs/isis/bin:$PATH

                            tabledump -HELP

                            ctest -R _module_ -j4 -VV
                            source deactivate

                        """
                    }
                }
                catch(e) {
                    build_ok = false
                    errors.add(env.STAGE_STATUS)
                    println e.toString()
                }

                try{
                    stage("GTests") {
                        env.STAGE_STATUS = "Running gtests on ${env.OS}"
                        sh """
                            source activate isis
                            echo $ISIS3TESTDATA
                            echo $ISIS3DATA

                            # environment variables
                            export ISISROOT=`pwd`
                            export ISIS3TESTDATA="/isisData/testData"
                            export ISIS3DATA='/isisData/data'
                            export PATH=`pwd`/bin:$PATH
                            which catlab

                            ctest -R "." -E "(_app_|_unit_|_module_)" -j4 -VV
                            source deactivate

                        """
                    }
                }
                catch(e) {
                    build_ok = false
                    errors.add(env.STAGE_STATUS)
                    println e.toString()
                }
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
