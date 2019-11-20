// vim: ft=groovy

node
{
    def rootDir = pwd()

    checkout scm

    sh "echo ${scm}"
    sh "ls -la ${pwd()}"
    sh "git log"

    def build_script = load "${rootDir}/script.groovy"

}
