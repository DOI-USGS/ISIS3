#!/usr/bin/env python

from pprint import pprint
import logging as log
import os
import json
import argparse
import subprocess
import tempfile

from os import path

log.basicConfig(
    format='%(asctime)s %(levelname)-8s %(message)s',
    level=log.DEBUG,
    datefmt='%Y-%m-%d %H:%M:%S')

# set log level to debug
def call_subprocess(command, redirect_stdout=True, redirect_stderr=False):
    stdout = subprocess.PIPE if redirect_stdout else None
    stderr = subprocess.PIPE if redirect_stderr else None
    with subprocess.Popen(
            command,
            stdout=stdout,
            stderr=stderr) as proc:
        (out, err) = proc.communicate()

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
            return call_subprocess(command_with_args) 
        else:
            log.debug(f"rclone() : using config string {config}")

            # most likely a config string
            with tempfile.NamedTemporaryFile() as f:  
                config_path = path.realpath(f.name)

                log.debug(f"USING CONFIG:\n{config}")
                
                f.write(config.encode())
                command_with_args = ["rclone", command, f"--config={config_path}", *extra_args]
                log.debug("Invoking : %s", " ".join(command_with_args))
                return call_subprocess(command_with_args, redirect_stdout, redirect_stderr) 
        
    except FileNotFoundError as not_found_e:
        log.error("Executable not found. %s", not_found_e)
        return {
            "code": -20,
            "error": not_found_e
        }
    except Exception as generic_e:
        log.exception("Error running command. Reason: %s", generic_e)
        return {
            "code": -30,
            "error": generic_e
        }


def driver_for_missions(command, mission_to_download, mission_list, cfg, destination, dry_run):
    """
    Parameters 
    ----------
    mission_to_download : str
        mission that you want to download 

    mission_list : list
        list of missions to loop though and get kernel's for mission to download 

    cfg : str
        config file for mission's ftp and http paths 

    destination str 
        path to location where files will be copied/downloaded too
    """
    path = None
    for mission in mission_list: 
        if mission_to_download.lower() == mission.strip(":"):
            path = f"{mission}/"
            log.info("Mission found in mission list")

    if path == None:
        log.info("Mission not found in mission list")
        raise RuntimeError(f"Mission not found in mission list: {mission_to_download}")

    else:
        download_pub(command, destination, path, cfg, dry_run)
             

def download_pub(inputcommand, destination, mission_name, cfg, dry_run):
    """
    Parameters 
    ----------
    
    destination str 
            path to location where files will be copied/downloaded too
    
    set_of_pub : set(str)
            set of kernels that will be downloaded
    
    cfg : str
            config file
    """
    
    log.info("Starting to download kernels")

    if(inputcommand == "lsf"):
        
        mission_name = mission_name.strip("/")
        log.debug(f"Running lsf using mission name {mission_name}")

        extra_args= [f"{mission_name}", "-R", "--format", "p", "--files-only"]
        results = rclone(command="lsf", extra_args=extra_args, config=cfg, redirect_stdout=True, redirect_stderr=False)
        f = open(f"{destination}/output.json" , "w")
        f.truncate()
        f.write(json.loads(json.dumps(results.get('out').decode('utf-8'))))
        f.close()
    else:
        destination += "/"+str(mission_name).replace(":","")
        destination = destination.replace("_usgs/","/")
        destination = destination.replace("_base/","/")
        extra_args=[f"{mission_name}",f"{destination}", "--progress", "--track-renames"]
    if dry_run:
        extra_args.append("--dry-run")


    rclone(command=inputcommand, extra_args=extra_args, config=cfg, redirect_stdout=False, redirect_stderr=False)
    log.info("Done downloading files")


def main(command, mission, dest, legacy_flag, dry_run):
    """
    Parameters 
    ----------
    
    source str 
            path to location where files are being copied/downloaded from
    
    dest : str
               path to location where files will be copied/downloaded too
    """
    log.info("---Script starting---")
    
    cfg_path = os.path.dirname(__file__) + '/../config/rclone.conf'

    log.debug(f"Using config: {cfg_path}")
    result = rclone("listremotes", config=cfg_path)
    results = result.get('out').decode("utf-8").split("\n")
    
    log.debug(f"Output from list remotes:\n {results}")

    if legacy_flag:
        legacy_remotes = ["legacy_base_usgs"]
        for remote in legacy_remotes:
            driver_for_missions(command, remote, results, cfg_path, dest, dry_run)

    if(mission.upper() == "ALL"):
        mission_arr =  ["tgo", "dawn", "cassini", "hayabusa2", "juno", "odyssey", "mro", "mex", "cassis", "apollo15", "apollo16", "apollo17", "base", "hayabusa", "chandrayaan1", "clementine1", "kaguya", "mariner10", "messenger", "mgs", "near", "newhorizons", "osirisrex", "rosetta", "smart1", "viking1", "viking2", "voyager1", "voyager2"]
        for mission in mission_arr:

            if f"{mission}_pub:" in results:
                driver_for_missions(command, f"{mission}_pub", results, cfg_path, dest, dry_run)

            if f"{mission}_usgs:" in results:
                driver_for_missions(command, f"{mission}_usgs", results, cfg_path, dest, dry_run)

    else:
        if f"{mission}_pub:" in results:
            driver_for_missions(command, f"{mission}_pub", results, cfg_path, dest, dry_run)

        if f"{mission}_usgs:" in results:
            driver_for_missions(command, f"{mission}_usgs", results, cfg_path, dest, dry_run)




if __name__ == '__main__':
    mission_arr =  ["tgo", "dawn", "cassini", "hayabusa2", "juno", "odyssey", "mro", "mex", "cassis", "apollo15", "apollo16", "apollo17", "base", "hayabusa", "chandrayaan1", "clementine1", "kaguya", "mariner10", "messenger", "mgs", "near", "newhorizons", "osirisrex", "rosetta", "smart1", "viking1", "viking2", "voyager1", "voyager2"]
    missionList = ""
    for mission in mission_arr:
        missionList += mission + ", "
    helpString = (
    '''This will allow for a user to download isis data directly to their machine from the USGS S3 buckets as well as public end points\n\n'''
    '''To use the download ISIS Data script you must supply 3 parameters with an optional 4th.\n\n'''
    '''<rclone command> <Mission name> <destination to copy to> <--dry-run (optional)> \nCompatible missions to download data from:'''
    f'''\n[{missionList}]\n\n'''
    '''Example of how to run this program:\n'''
    '''python downloadIsisData.py sync tgo ~/isisData/tgo\n\n'''
    '''NOTE: if you would like to download the data for every mission supply the value ALL for the <Mission name> parameter''')
    parser = argparse.ArgumentParser(description = helpString, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('command', help="the rclone command you would like to run; ie lsjson, sync")
    parser.add_argument('mission', help='mission for files to be downloaded')
    parser.add_argument('dest', help='the destination to download files from source')
    parser.add_argument('--legacy', help="flag to download ISIS data prior to ISIS 4.1.0", default=False, action='store_true')
    parser.add_argument('--dry-run', help="run a dry run for rclone value should be a boolean", default=False, action='store_true')
    parser.add_argument('-v', '--verbose', action='count', default=0)    
    args = parser.parse_args()

    if args.verbose == 1:
        log.basicConfig(
            format='%(asctime)s %(levelname)-8s %(message)s',
            level=log.INFO,
            datefmt='%Y-%m-%d %H:%M:%S')

    if args.verbose >= 2:
        log.basicConfig(
            format='%(asctime)s %(levelname)-8s %(message)s',
            level=log.DEBUG,
            datefmt='%Y-%m-%d %H:%M:%S')

    main(args.command, args.mission, args.dest, args.legacy ,args.dry_run)

