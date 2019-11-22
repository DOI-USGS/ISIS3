// vim: ft=groovy

pipeline
{
    agent any
    stages {
        stage ("CI") {
            steps {
                script {
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
                                    sh 'ls -lah ${PWD}'
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
