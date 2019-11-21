// vim: ft=groovy

pipeline
{
    agent any
    stages {
        stage ("CI") {
            parallel {
                stage ("CentOS") {
                    node("centos") {
                        stage ("Create Environment") {
                            steps {
                                script {
                                    def rootDir = pwd()
                                    def build_script = load "${rootDir}/script.groovy"

                                    build_script.myFunc("centos")
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
