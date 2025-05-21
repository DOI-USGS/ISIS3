#!/bin/bash

# Define environment name and package to install
DOWNLOAD_DATA="YES"
CLIENT="mamba"
FORCE_MAMBA_INSTALL="NO"  # New flag to force mamba installation

# Report error and reserves error code from a failed command
failed_command() {
    local _status=$?
    if [ "${_status}" -ne 0 ]; then 
        echo "Error: $1 failed" 
        exit ${_status}
    fi
}

# Apply check for optional arguments that expect a value
check_valid_arg() {
    if [[ "$2" = "-"* ]]; then
        echo "Invalid argument: $2"
        echo "Found after $1"
        exit 1
    fi

    if [[ -z $2 ]]; then
        echo "No value for $1 provided"
        exit 1
    fi
}

print_help() {
    printf "Usage: $0 [options]\n"
    printf "Options:\n"
    printf "\t-h, --help            Show this help message and exit\n"
    printf "\t-l, --anaconda-label  Different ISIS labels as defined by the anaconda\n"
    printf "\t                      labels at https://anaconda.org/usgs-astrogeology/\n"
    printf "\t                      isis, examples include \"LTS\", \"dev\", and \"RC\"\n"
    printf "\t-v, --isis-version    Different ISIS versions as defined by the\n"
    printf "\t                      anaconda versions for a label at\n"
    printf "\t                      https://anaconda.org/usgs-astrogeology/isis,\n"
    printf "\t                      examples include 8.0.3 (LTS/Feature),\n"
    printf "\t                      8.2.0_RC1 (RC), and 2025.02.22 (dev)\n"
    printf "\t-n, --env-name        The name of the anaconda environment to create.\n"
    printf "\t-m, --miniforge-dir   Define the directory to an anaconda package\n"
    printf "\t                      manager install location. If you have an\n"
    printf "\t                      anaconda package manager already this\n"
    printf "\t                      argument will be ignored. If not a version\n"
    printf "\t                      of miniforge will be installed at this\n"
    printf "\t                      location\n"
    printf "\t-p, --data-prefix     The directory where ISISDATA is located. If\n"
    printf "\t                      this directory doesn't exist then one will\n"
    printf "\t                      be made at its location\n"
    printf "\t--no-data             Do not ask to download any data for the\n"
    printf "\t                      ISIS_DATA area.\n"
    printf "\t--download-base       Download the base data without prompting the user.\n"
    printf "\t--force-mamba         Force installation of mamba regardless of existing conda\n"
    printf "\n"
    printf "\tDefining variables on the command line will skip the\n"
    printf "\tinteractive elements within this script\n"
    printf "\n\n"
}

for arg in "$@"; do
    case $arg in
        -h|--help)
            print_help
            exit 0
            ;;
    esac
done

POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -l|--anaconda-label)
            check_valid_arg $1 $2
            ANACONDA_LABEL="$2"
            shift # past argument
            shift # past value
            ;;
        -v|--isis-version)
            check_valid_arg $1 $2
            ISIS_VERSION="$2"
            shift # past argument
            shift # past value
            ;;
        -n|--env-name)
            check_valid_arg $1 $2
            ENV_NAME="$2"
            shift # past argument
            shift # past value
            ;;
        -m|--miniforge-dir)
            check_valid_arg $1 $2
            MINIFORGE_DIR="$2"
            shift # past argument
            shift # past value
            ;;
        -p|--data-prefix)
            check_valid_arg $1 $2
            ISISDATA_PREFIX="$2"
            shift # past argument
            shift # past value
            ;;
        --no-data)
            DOWNLOAD_DATA=NO
            shift # past argument
            ;;
        --download-base)
            DOWNLOAD_DATA=YES
            BASE_DATA_ONLY=YES
            shift # past argument
            ;;
        --force-mamba)
            FORCE_MAMBA_INSTALL="YES"
            shift
            ;;
        -*|--*)
            echo "Unknown option $1"
            exit 1
            ;;
        *)
            POSITIONAL_ARGS+=("$1") # save positional arg
            shift # past argument
            ;;
    esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

# If an ANACONDA_LABEL was not set, ask the user for it
if [[ -z $ANACONDA_LABEL ]]; then
    ANACONDA_LABEL="main"
    printf "ISIS anaconda label [$ANACONDA_LABEL] will be installed.\n"
    printf "\\n"
    printf "  - Press ENTER to confirm the ISIS label to install\\n"
    printf "  - Press CTRL-C to abort the installation\\n"
    printf "  - Or specify a different ISIS label below [main, dev, lts, rc]\\n"
    printf "  - Default is [main]\\n"
    printf "\\n"
    printf "[%s] >>> " "$ANACONDA_LABEL"

    read -r anaconda_label

    # If a label was given, set it
    if [ -n "$anaconda_label" ]; then
        ANACONDA_LABEL=$anaconda_label
    fi
fi

# Force label to uppercase for LTS and RC
if [ "${ANACONDA_LABEL,,}" = "lts" ] || [ "${ANACONDA_LABEL,,}" = "rc" ]; then
    ANACONDA_LABEL="${ANACONDA_LABEL^^}"
fi

printf "\nISIS anaconda label set to [$ANACONDA_LABEL]\n"

if [ "$ANACONDA_LABEL" = "main" ]; then
    PACKAGE_NAME="usgs-astrogeology::isis"
else
    PACKAGE_NAME="usgs-astrogeology/label/$ANACONDA_LABEL::isis"
fi

# If an ISIS_VERSION was not set, ask the user for it
if [[ -z $ISIS_VERSION ]]; then
    ISIS_VERSION="latest"
    # Skip the version prompt if ANACONDA_LABEL is dev
    if [ "$ANACONDA_LABEL" != "dev" ]; then
        printf "Specify an ISIS version to install from the ISIS anaconda label [$ANACONDA_LABEL]\n"
        printf "\\n"
        printf "  - Press ENTER to install the latest version of ISIS found under [$ANACONDA_LABEL]\\n"
        printf "  - Press CTRL-C to abort the installation\\n"
        printf "  - Or specify an ISIS version below\\n"
        printf "\\n"
        printf "[%s] >>> " "$ISIS_VERSION"

        read -r isis_version

        # If a label was given, set it
        if [ -n "$isis_version" ]; then
            ISIS_VERSION=$isis_version
        fi
        printf "\nISIS version set to [$ISIS_VERSION]\n"
    else
        printf "\nUsing latest version from dev channel\n"
        ISIS_VERSION="latest"
    fi
else
    printf "\nISIS version set to [$ISIS_VERSION]\n"
fi

if [ $ISIS_VERSION = "latest" ]; then
    ISIS_VERSION=""
fi

printf "\nBeginning install of $PACKAGE_NAME\n\n"

# Determine the OS type
case "$(uname)" in
    "Linux")
        MINIFORGE_INSTALLER="Miniforge3-Linux-x86_64.sh"
        LIBGL_INSTALL=""
        ;;
    "Darwin")
        MINIFORGE_INSTALLER="Miniforge3-MacOSX-x86_64.sh"
        LIBGL_INSTALL=""
        export CONDA_SUBDIR=osx-64
        ;;
    *)
        echo "Unsupported OS: $(uname)"
        exit 1
        ;;
esac

# Check if conda is installed but mamba is not, or if forced to install mamba
if command -v conda &> /dev/null && ! command -v mamba &> /dev/null; then
    if [ "$FORCE_MAMBA_INSTALL" = "YES" ]; then
        echo "Forcing mamba installation."
    else
        echo "WARNING: conda is installed but mamba is not."
        echo "mamba is a faster alternative to conda and is required for ISIS installation."
        echo "Otherwise, environment creation will take much longer than what is practical."
        echo "ISIS only supports mamba for installation, you can install it now to move forward."
        echo "Would you like to install mamba? [yes|no]"
        ans="yes"
        printf "[%s] >>> " "$ans"
        
        read -r ans
        
        # If no input, use default
        if [ "$ans" = "" ]; then
            ans="yes"
        fi
        
        ans=$(echo "${ans}" | tr '[:lower:]' '[:upper:]')
        while [ "$ans" != "YES" ] && [ "$ans" != "NO" ]
        do
            echo "Please answer 'yes' or 'no':"
            printf ">>> "
            read -r ans
            ans=$(echo "${ans}" | tr '[:lower:]' '[:upper:]')
        done
        
        if [ "$ans" == "NO" ]; then
            echo "Error: Only mamba is supported. Exiting."
            exit 1
        else
            echo "Will proceed with installing mamba."
        fi
    fi
fi

# Install Miniforge if it's not already installed
if ! command -v mamba &> /dev/null; then

    echo "Miniforge not found, installing Miniforge..."

    # If a MINIFORGE_DIR is not set, ask the user
    if [ -z "$MINIFORGE_DIR" ]; then
        MINIFORGE_DIR="$HOME/miniforge3"

        printf "Miniforge will be installed at this location:\n\n" 
        printf "\t$MINIFORGE_DIR/\n\n"
        printf "\\n"
        printf "  - Press ENTER to confirm the Miniforge install location\\n"
        printf "  - Press CTRL-C to abort the installation\\n"
        printf "  - Or specify a different location below\\n"
        printf "\\n"
        printf "[%s] >>> " "$MINIFORGE_DIR"

        read -r miniforge_install_path

        # If input was given, set it
        if [ -n "$miniforge_install_path" ]; then
            MINIFORGE_DIR=$miniforge_install_path
        fi
    fi

    # Check if MINIFORGE_DIR exists but mamba is not in PATH
    if [ -d "$MINIFORGE_DIR" ]; then
        echo "Miniforge directory exists at $MINIFORGE_DIR, reusing existing installation"
    else
        curl -kL "https://github.com/conda-forge/miniforge/releases/latest/download/$MINIFORGE_INSTALLER" -o Miniforge3.sh || failed_command "Miniforge download"
        bash Miniforge3.sh -b -p $MINIFORGE_DIR || failed_command "Miniforge installation"
    fi

    $MINIFORGE_DIR/bin/$CLIENT init || exit 1
    export PATH="$MINIFORGE_DIR/bin:$PATH"
else
    echo "Miniforge is already installed."
    if [[ "$CONDA_PREFIX" == *"/envs/"* ]]; then
        MINIFORGE_DIR="${CONDA_PREFIX%/*/*}"
    else
        MINIFORGE_DIR=$CONDA_PREFIX
    fi
    echo "Setting MINIFORGE_DIR to $MINIFORGE_DIR"
fi

# If an ENV_NAME was not set, ask the user for it
if [ -z "$ENV_NAME" ]; then
    ENV_NAME="isis"
    printf "ISIS will be installed at this location:\n\n" 
    printf "\t$MINIFORGE_DIR/envs/$ENV_NAME\n\n"
    printf "\\n"
    printf "  - Press ENTER to confirm the ISIS install environment name\\n"
    printf "  - Press CTRL-C to abort the installation\\n"
    printf "  - Or specify a different environemnt name below\\n"
    printf "\\n"
    printf "[%s] >>> " "$ENV_NAME"

    read -r isis_env_name

    # If a name was given, set it
    if [ -n "$isis_env_name" ]; then
        ENV_NAME=$isis_env_name
    fi
fi

# Check if the environment already exists 
if $CLIENT env list | grep -qE "^$ENV_NAME[ ]."; then 
    printf "\nEnvironment \"$ENV_NAME\" already exists. Not performing any updates.\n" 
    printf "To delete the old environment, use the following commands:\n\n" 
    printf "\t$ mamba deactivate\n" 
    printf "\t$ mamba remove -n $ENV_NAME --all\n\n" 
else
    # Create a new environment with the specified package
    echo "Creating a new environment [$ENV_NAME] and installing $PACKAGE_NAME"
    $CLIENT create -c conda-forge -c usgs-astrogeology -n $ENV_NAME $PACKAGE_NAME=$ISIS_VERSION $LIBGL_INSTALL rclone -y || {
        echo "Failed to install $PACKAGE_NAME=$ISIS_VERSION"
        echo "Seaching for $PACKAGE_NAME versions ..."
        $CLIENT search -c conda-forge -c usgs-astrogeology $PACKAGE_NAME || failed_command "Search for $PACKAGE_NAME versions"
        exit 1
    }
    echo "Environment \"$ENV_NAME\" created and $PACKAGE_NAME installed."
fi

export PATH="$MINIFORGE_DIR/envs/$ENV_NAME/bin:$PATH"

# If a ISISDATA_PREFIX was not set, ask the user for it
if [ -z "$ISISDATA_PREFIX" ]; then
    ISISDATA_PREFIX="$HOME/.Isis/isis_data"
    printf "ISISDATA directory is needed for most applications and is several Gbs in size. \n"
    printf "By default, ISISDATA will be installed at this location:\n\n"
    printf "\t$ISISDATA_PREFIX\n"
    printf "\\n"
    printf "  - Press ENTER to confirm the ISISDATA location\\n"
    printf "  - Press CTRL-C to abort the installation (this will skip data installation)\\n"
    printf "  - Or specify a different location below\\n"
    printf "\\n"
    printf "[%s] >>> " "$ISISDATA_PREFIX"

    read -r isis_data_path

    # If no input, use default path
    if [ -n "$isis_data_path" ]; then
        ISISDATA_PREFIX=$(eval printf "%s" $isis_data_path)
    fi

    echo "Setting ISISDATA to $ISISDATA_PREFIX"
fi

# Verify if the folder exists
if [ ! -d "$ISISDATA_PREFIX" ]; then
    if [ "$DOWNLOAD_DATA" = "YES" ]; then
        echo "Creating folder $ISISDATA_PREFIX"
        mkdir -p $ISISDATA_PREFIX || failed_command "Creating $ISISDATA_PREFIX"
    else
        echo "Not creating directory $ISISDATA_PREFIX since the --no_data flag was set"
    fi
fi

$CLIENT env config vars set -n $ENV_NAME ISISDATA=$ISISDATA_PREFIX ISISROOT=$MINIFORGE_DIR/envs/$ENV_NAME || failed_command "Mamba config var set"

if [[ "$MINIFORGE_DIR" == *"/envs/$ENV_NAME"* ]]; then
    ENV_PATH="$MINIFORGE_DIR"
else
    ENV_PATH="$MINIFORGE_DIR/envs/$ENV_NAME"
fi

if [ ! "$DOWNLOAD_DATA" = "NO" ]; then
    DOWNLOAD_ISIS_DATA_SCRIPT="$ENV_PATH/bin/downloadIsisData"
    if [[ ! -f "$ENV_PATH/bin/downloadIsisData" && ! -f "$ENV_PATH/etc/isis/rclone.conf" ]]; then
        if ! [ -f "$DOWNLOAD_ISIS_DATA_SCRIPT" ]; then
            printf "\nInstalled download script: $DOWNLOAD_ISIS_DATA_SCRIPT\n"
            curl  --output "$ENV_PATH/bin/downloadIsisData" -LJO https://github.com/DOI-USGS/ISIS3/raw/dev/isis/scripts/downloadIsisData
            chmod +x "$ENV_PATH/bin/downloadIsisData"
        fi
        if ! [ -f "$ENV_PATH/etc/isis/rclone.conf" ]; then
            printf "\nInstalled isis rclone config: $ENV_PATH/etc/isis/rclone.conf\n"
            # Verify if the folder exists
            if ! [ -d "$ENV_PATH/etc/isis/" ]; then
                echo "Creating folder $ISISDATA_PREFIX"
                mkdir -p $ENV_PATH/etc/isis/ || failed_command "Creating $ENV_PATH/etc/isis/"
            fi
            curl --output "$ENV_PATH/etc/isis/rclone.conf" -LJO https://github.com/DOI-USGS/ISIS3/raw/dev/isis/config/rclone.conf
        fi
    else
        printf "\nFound download script: $DOWNLOAD_ISIS_DATA_SCRIPT\n"
    fi

    printf "\n\n"
    echo "ISISDATA is required for most applications."
    echo "This will download several gigabytes of data and may take a few hours."
    echo "ISISDATA path is currently set to $ISISDATA_PREFIX"

    if [ -n "$BASE_DATA_ONLY" ]; then
        echo "[Running] downloadIsisData base $ISISDATA_PREFIX"
        $MINIFORGE_DIR/envs/$ENV_NAME/bin/downloadIsisData -n 20 base $ISISDATA_PREFIX || failed_command "ISISDATA base download"
    else
        echo "You can do this later, read more at:" 
        printf "\n\thttps://astrogeology.usgs.gov/docs/how-to-guides/environment-setup-and-maintenance/isis-data-area/\n\n"
        echo "Do you want to install base ISISDATA now? This can be done later. [yes|no]"
        ans="no"
        printf "[%s] >>> " "$ans"
        read -r ans
        
        # If no input, use default path
        if [ "$ans" = "" ]; then
            ans="no"
        fi

        ans=$(echo "${ans}" | tr '[:lower:]' '[:upper:]')
        while [ "$ans" != "YES" ] && [ "$ans" != "NO" ]
        do
            echo "Please answer 'yes' or 'no':"
            printf ">>> "
            read -r ans
            ans=$(echo "${ans}" | tr '[:lower:]' '[:upper:]')
        done

        if [ "$ans" == "YES" ]; then
            echo "[Running] downloadIsisData base $ISISDATA_PREFIX"
            $MINIFORGE_DIR/envs/$ENV_NAME/bin/downloadIsisData -n 20 base $ISISDATA_PREFIX || failed_command "ISISDATA base download"
        fi 

        if [ "$ans" == "NO" ]; then
            printf "\n"
            printf "You can download base ISISDATA later with\n" 
            printf "\tdownloadIsisData base \$ISISDATA\n"
        fi
    fi
    if [ "$ans" == "YES" ]; then
        printf "\n"
        printf "Do you want to install mission-specific ISISDATA now? This can be done later. [yes|no]\n"
        ans="no"
        printf "[%s] >>> " "$ans"

        read -r ans

        # If no input, use default path
        if [ "$ans" = "" ]; then
            ans="no"
        fi

        ans=$(echo "${ans}" | tr '[:lower:]' '[:upper:]')
        while [ "$ans" != "YES" ] && [ "$ans" != "NO" ]
        do
            printf "Please answer 'yes' or 'no':"
            printf ">>> "
            read -r ans
            ans=$(echo "${ans}" | tr '[:lower:]' '[:upper:]')
        done

        if [ "$ans" == "YES" ] ; then
            printf "Enter 1 or more mission names, separated by spaces and then press ENTER\n"
            printf "Available missions are\\n"
            printf "\tapollo15   apollo16      apollo17
        \tcassini    chandrayaan1  clementine1
        \tdawn       tgo           galileo
        \thayabusa2  juno          kaguya
        \tlo         lro           mer
        \tmariner10  messenger     mex
        \tmgs        mro           msl
        \todyssey    near          newhorizons
        \tosirisrex  rolo          rosetta
        \tsmart1     viking1       viking2\n"
            printf ">>> "
            read -r missions
            missions=$(echo "${missions}" | tr '[:upper:]' '[:lower:]')
            # Convert to array and loop over missions to download
            # No validation, let mistyped missions fail naturally
            IFS=' ' read -a mission_arr <<< "$missions"
            for i in ${mission_arr[@]} ; do
                echo "[Running] downloadIsisData ${i} $ISISDATA_PREFIX"
                $MINIFORGE_DIR/envs/$ENV_NAME/bin/downloadIsisData -n 20 ${i} "$ISISDATA_PREFIX"/ || failed_command "ISISDATA ${i} download"
            done
        fi
    else
        printf "\n"
        printf "You can download mission specific ISISDATA later with\n" 
        printf "\tdownloadIsisData [mission_name] \$ISISDATA"
    fi
fi

printf "\n\n"
printf "Activate the new environment with:\n\n"
if ! command -v mamba &> /dev/null ; then
    printf "\t$ source ~/.bashrc  # or restart your shell\n"
fi
printf "\t$ conda activate $ENV_NAME\n\n"

