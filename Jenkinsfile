// vim: ft=groovy

pipeline
{
    agent any
    stages
    {
        stage('Testing')
        {
            def rootDir = pwd()
            def build_script = load "${rootDir}/build.groovy"
            steps
            {
                script
                {
                    build_script.my_func()
                }
            }
        }
    }
}
