// vim: ft=groovy

def condaPath = ""

def isisEnv = [
    "ISIS3DATA=/isisData/data",
    "ISIS3TESTDATA=/isisData/testData",
    "ISIS3MGRSCRIPTS=/isisData/data/isis3mgr_scripts",
]

def cmakeFlags = [
    "-DJP2KFLAG=ON",
    "-DKAKADU_INCLUDE_DIR=/isisData/kakadu",
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
        sh 'git config --global http.sslVerify false'
        checkout scm
        isisEnv.add("ISISROOT=${pwd()}/build")
        cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")
    }

    stage("Create environment") {
        
        env.STAGE_STATUS = "Creating conda environment"
        
        if (env.OS.toLowerCase() == "mac") {
          condaPath = "/tmp/" + sh(script: '{ date "+%m/%d/%y|%H:%M:%S:%m"; echo $WORKSPACE; } | md5 | tr -d "\n";', returnStdout: true) 

          sh """
            curl -o miniconda.sh  https://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
            bash miniconda.sh -b -p ${condaPath}
            """
        } else {
          condaPath = "/home/jenkins/.conda/"
        } 
 
        isisEnv.add("PATH=${pwd()}/install/bin:$condaPath/envs/isis/bin:$condaPath/bin:${env.PATH}")
        
        withEnv(isisEnv) {

          println("Complete Environment:")
          sh 'printenv'
          println("Anaconda Path: " + condaPath)
          
          sh """
              # Use the conda cache running on the Jenkins host
              # conda config --set channel_alias http://dmz-jenkins.wr.usgs.gov
              conda config --set always_yes True
              conda config --set ssl_verify false 
              conda update -n base -c defaults conda
          """
           
          
          if (env.OS.toLowerCase() == "centos") {
              sh 'conda env create -n isis -f environment_gcc4.yml'
          } else {
            sh """
                conda config --show channels
                conda env create -n isis -f environment.yml
            """
          }
       } 
    } 

    withEnv(isisEnv) {
        dir("${env.ISISROOT}") {
            try {
                stage ("Build") {
                    env.STAGE_STATUS = "Building ISIS on ${env.OS}"
                    sh """
                        source activate ${condaPath}/envs/isis
                        echo `ls ../`
                        echo `pwd`
                        conda list
                        cmake -GNinja ${cmakeFlags.join(' ')} ../isis
                        ninja -j4 install
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
                                    source activate ${condaPath}/envs/isis
                                    ctest -R _unit_ -j4 -VV
                                """

                        }
                    }
                }
                catch(e) {
                    build_ok = false
                    echo e.toString()
                }
                sh 'source deactivate'

                try{
                    stage("AppTests") {
                        env.STAGE_STATUS = "Running app tests on ${env.OS}"
                        sh """
                            source activate ${condaPath}/envs/isis
                            echo $PATH
                            ctest -R _app_ -j4 -VV
                        """
                    }
                }
                catch(e) {
                    build_ok = false
                    errors.add(env.STAGE_STATUS)
                    println e.toString()
                }
                sh 'source deactivate'

                try{
                    stage("ModuleTests") {
                        env.STAGE_STATUS = "Running module tests on ${env.OS}"
                        sh """
                            source activate ${condaPath}/envs/isis 
                            ctest -R _module_ -j4 -VV
                        """
                    }
                }
                catch(e) {
                    build_ok = false
                    errors.add(env.STAGE_STATUS)
                    println e.toString()
                }
                sh 'source deactivate'

                try{
                    stage("GTests") {
                        env.STAGE_STATUS = "Running gtests on ${env.OS}"
                        sh """
                            source activate ${condaPath}/envs/isis
                            ctest -R "." -E "(_app_|_unit_|_module_)" -j4 -VV
                        """
                    }
                }
                catch(e) {
                    build_ok = false
                    errors.add(env.STAGE_STATUS)
                    println e.toString()
                }
                sh 'source deactivate'
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

    stage("Clean Up") {
      env.STAGE_STATUS = "Removing conda environment"
    }
}
