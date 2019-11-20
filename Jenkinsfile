// vim: ft=groovy

pipeline
{
    agent any

    stages {
        stage ("Create Environment") {
            steps {
                script {
                    def rootDir = pwd()
                    def build_script = load "${rootDir}/script.groovy"

                    sh "echo ${pwd()}"

                    build_script.myFunc("centos")
                }
            }
        }
    }
}
