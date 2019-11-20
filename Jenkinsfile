// vim: ft=groovy

node
{
    def rootDir = pwd()

    sh "ls -la ${pwd()}"
    sh "git log"

    def build_script = load "${rootDir}/script.groovy"

}
