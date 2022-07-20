#!/usr/bin/env python

from multiprocessing import ProcessError
from multiprocessing.dummy import Process
from pprint import pprint
import logging as log
import os
import json
import argparse
import subprocess
import tempfile
from shutil import which
from os import path


def find_conf():
    from pathlib import Path
    local_path = Path("rclone.conf")
    install_path = Path(os.environ.get("CONDA_PREFIX")) / "etc" / local_path
    repo_path = Path(os.path.dirname(__file__)) / '..' / 'config' / 'rclone.conf' 
    
    if local_path.exists():
        return str(local_path) 
    elif repo_path.exists():
        return str(repo_path)
    elif install_path.exists():
        return str(install_path)
    else:
        return ""


# set log level to debug
def call_subprocess(command, redirect_stdout=True, redirect_stderr=False):
    stdout = subprocess.PIPE if redirect_stdout else None
    stderr = subprocess.PIPE if redirect_stderr else None

    if not which(command[0]):
        raise ProcessLookupError(f"{command[0]} is not installed.")

    log.debug("Invoking : %s", " ".join(command))

    with subprocess.Popen(
            command,
            stdout=stdout,
            stderr=stderr) as proc:
        (out, err) = proc.communicate()

        if out:
            log.debug("Process output: ")
            log.debug(out.decode())
        if err:
            log.warning(err.decode("utf-8").replace("\\n", "\n"))

        return {
            "code": proc.returncode,
            "out": out,
            "error": err
        }


def rclone(command, config=None, extra_args=[], redirect_stdout=True, redirect_stderr=False):
    try:
        if path.isfile(config):
            log.debug(f"rclone() : using config file {config}")

            # this is probably a config file on disk so pass it through
            config_path = config
            command_with_args = ["rclone", command, f'--config={config_path}', *extra_args]
            log.debug("Invoking : %s", " ".join(command_with_args))
            return call_subprocess(command_with_args, redirect_stdout, redirect_stderr)
        else:
            log.debug(f"rclone() : using config string {config}")

            # most likely a config string
            with tempfile.NamedTemporaryFile() as f:
                config_path = path.realpath(f.name)

                log.debug(f"USING CONFIG:\n{config}")

                f.write(config.encode())
                command_with_args = ["rclone", command, f"--config={config_path}", *extra_args]
                return call_subprocess(command_with_args, redirect_stdout, redirect_stderr)
    except ProcessLookupError as not_found_e:
        log.error("Executable not found. %s", not_found_e)
        raise Exception("rclone is not installed, please install with: 'conda install -c conda-forge rclone'")
    except Exception as generic_e:
        log.exception("Error running command. Reason: %s", generic_e)
        raise Exception()


def create_rclone_arguments(destination, mission_name, dry_run=False, ntransfers=10):
    """
    Parameters
    ----------

    destination str
            path to location where files will be copied/downloaded too

    set_of_pub : set(str)
            set of kernels that will be downloaded
    """
    log.debug(f"Creating RClone command for {mission_name}")
    mission_dir_name, source_type = mission_name.replace(":", "").split("_")

    log.debug(f"Mission_dir_name: {mission_dir_name}, source_type: {source_type}")

    destination += str(mission_dir_name).replace(":","")
    if source_type == "naifKernels":
        destination = os.path.join(destination, "kernels")

    if not os.path.exists(destination) and dry_run==False:
        log.debug("{destination} does not exist, making directory")
        os.makedirs(destination)

    extra_args=[f"{mission_name}",f"{destination}", "--progress", f"--checkers={ntransfers}", f"--transfers={ntransfers}", "--track-renames", f"--log-level={log.getLevelName(log.getLogger().getEffectiveLevel())}"]
    
    if dry_run:
        extra_args.append("--dry-run")

    log.debug(f"Args created: {extra_args}")
    return extra_args


def main(mission, dest, cfg_path, dry_run, ntransfers):
    """
    Parameters
    ----------

    source str
            path to location where files are being copied/downloaded from

    dest : str
               path to location where files will be copied/downloaded too
    """
    log.info("---Script starting---")

    log.debug(f"Using config: {cfg_path}")
    result = rclone("listremotes", config=cfg_path)
    config_sources = result.get('out').decode("utf-8").split("\n")
    if config_sources == ['']: 
        log.error("Remote sources came back empty. Get more info by re-running with verbose flag.")
        quit(-1)
    log.debug(f"Remote Sources: {config_sources}")

    supported_missions = {}
    for source in config_sources: 
        parsed_name = source.split("_")
        # If it is a mission, it should be in the format <mission_nam>_<source_type>
        if len(parsed_name) == 2 and parsed_name[1] in ["usgs:", "naifKernels:"]: 
            remotes_mission_name = parsed_name[0]
            supported_missions[remotes_mission_name] = supported_missions.get(remotes_mission_name, []) + [source]

    log.debug(f"Supported missions:\n {supported_missions.keys()}")
    log.debug(f"Complete Dictionary:\n {json.dumps(supported_missions, indent=2)}")

    if not mission in supported_missions.keys():
        raise LookupError(f"{mission} is not in the list of supported missions: {supported_missions.keys()}")
 
    if mission == "legacybase":
        args = create_rclone_arguments(dest, "legacybase_usgs:", dry_run, ntransfers)
        rclone(command="sync", extra_args=args, config=cfg_path, redirect_stdout=False, redirect_stderr=False)
    elif(mission.upper() == "ALL"):
        supported_missions.pop("legacybase")
        for mission, remotes in supported_missions.items():
            for remote in remotes:
                args = create_rclone_arguments(dest, remote, dry_run, ntransfers)
                rclone(command="sync", extra_args=args, config=cfg_path, redirect_stdout=False, redirect_stderr=False)
    else:
        for remote in supported_missions[mission]:
            args = create_rclone_arguments(dest, remote, dry_run, ntransfers)
            rclone(command="sync", extra_args=args, config=cfg_path, redirect_stdout=False, redirect_stderr=False) 


if __name__ == '__main__': 
    helpString = (
    '''This will allow for a user to download isis data directly to their machine from the USGS S3 buckets as well as public end points\n\n'''
    '''To use the download ISIS Data script you must supply 3 parameters with an optional 4th.\n\n'''
    '''<rclone command> <Mission name> <destination to copy to> <--dry-run (optional)> \n'''
    '''Example of how to run this program:\n'''
    '''python downloadIsisData.py sync tgo ~/isisData/tgo\n\n'''
    '''NOTE: if you would like to download the data for every mission supply the value ALL for the <Mission name> parameter''')
    parser = argparse.ArgumentParser(description = helpString, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('mission', help='mission for files to be downloaded')
    parser.add_argument('dest', help='the destination to download files from source')
    parser.add_argument('--dry-run', help="run a dry run for rclone value should be a boolean", default=False, action='store_true')
    parser.add_argument('-v', '--verbose', action='count', default=0)
    parser.add_argument('-n', '--num-transfers', action='store', default=10)
    parser.add_argument('--config', action='store', default=find_conf())
    args = parser.parse_args()

    log_kwargs = {
      'format': '%(asctime)s %(levelname)-8s %(message)s',
      'level': log.WARN,
      'datefmt' : '%Y-%m-%d %H:%M:%S'
    }

    if args.verbose == 0:
        log_kwargs['level'] = log.WARN
    elif args.verbose == 1:
        log_kwargs['level'] = log.INFO
    elif args.verbose >= 2:
        log_kwargs['level'] = log.DEBUG

    log.basicConfig(**log_kwargs)

    main(args.mission, args.dest, os.path.expanduser(args.config), args.dry_run, args.num_transfers)
