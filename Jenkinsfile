// vim: ft=groovy

node
{
    def rootDir = pwd()
    def build_script = load "${rootDir}/script.groovy"
    
    stages {
        stage ("Checkout") {
            env.STAGE_STATUS = "Checking out ISIS"
            checkout scm
            isisEnv.add("ISISROOT=${pwd()}/build")
            cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")
        }

        build_script.myFunc(centos)
    }
}
