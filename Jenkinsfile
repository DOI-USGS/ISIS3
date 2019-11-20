// vim: ft=groovy

pipeline
{
    agent any
    stages
    {
        stage('Testing')
        {
            println("Apple")
            def rootDir = pwd()
            def build_script = load "${rootDir}/build.groovy"

            script
            {
                build_script.my_func()
            }
        }
    }
}
