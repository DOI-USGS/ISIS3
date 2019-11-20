// vim: ft=groovy

pipeline
{
    stages {
        stage ("Checkout") {
            checkout scm
            isisEnv.add("ISISROOT=${pwd()}/build")
            cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")
        }
        stage ("Create Environment") {
            steps {
                def rootDir = pwd()
                def build_script = load "${rootDir}/script.groovy"

                build_script.myFunc(centos)
            }
        }
    }
}
