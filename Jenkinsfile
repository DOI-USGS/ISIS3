// vim: ft=groovy

pipeline
{
    agent any

    def rootDir = pwd()
    def build_script = load "${rootDir}/build.groovy"
    
    stages
    {
        stage('Testing')
        {
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
