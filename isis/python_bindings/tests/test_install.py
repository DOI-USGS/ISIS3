import importlib.util
import sys
import subprocess


def test_module_import(): 
    name = 'astroset'
    if name in sys.modules:
        print(f"{name!r} already in sys.modules")
    assert (spec := importlib.util.find_spec(name)) is not None

def test_check_pip_install():
    package_name = "astroset"
    try:
        # Check if the package is installed by running the "pip show" command
        result = subprocess.check_output(['pip', 'show', package_name])
        package_info = result.decode('utf-8')
        
        # Parse the package information to check if it contains the package name
        assert(package_name.lower() in package_info.lower())
            
    except subprocess.CalledProcessError:
        assert False 

def test_app_install():
    app_name = 'findFeaturesSegment.py'
    try:
        # Check if the package is installed by running the "pip show" command
        result = subprocess.check_output([app_name, '-H'])
        help_string = result.decode('utf-8')
        
        # Parse the package information to check if it contains the package name
        assert("from" in help_string.lower())
        assert("fromlist" in help_string.lower())
            
    except subprocess.CalledProcessError:
        assert False 
         