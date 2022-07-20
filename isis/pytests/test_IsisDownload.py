from functools import total_ordering
from tkinter.filedialog import test
import pytest
import os

import sys
sys.path.append('../scripts/')

import json
import argparse
from downloadIsisData import rclone
import logging as log 

log.basicConfig(
    format='%(asctime)s %(levelname)-8s %(message)s',
    level=log.DEBUG,
    datefmt='%Y-%m-%d %H:%M:%S')

mission_arr =  ["tgo", "dawn", "cassini", "hayabusa2", "juno", "odyssey", "mro", "mex", "apollo15", "apollo16", "apollo17", "base", "hayabusa", "chandrayaan1", "clementine1", "kaguya", "mariner10", "messenger", "mgs", "near", "newhorizons", "osirisrex", "rosetta", "smart1", "viking1", "viking2", "voyager1", "voyager2"]
@pytest.mark.parametrize("mission_name",mission_arr)
def test(mission_name):
    log.debug(f"Testing mission: {mission_name}")
    cfg_path = (os.path.dirname(__file__) + '/../config/rclone.conf')
    log.info(cfg_path)
    dataPath = str(os.environ.get("ISISDATA"))

    log.debug("Getting Data from nfs mounts")
    # data from rclone with USGS Mount 
    truth_data = rclone("lsf", cfg_path, extra_args=[dataPath, "-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)
    truth_data = truth_data.get('out').decode('utf-8')
    truth_set = set()

    for line in truth_data.split("\n"):
        truth_set.add(line)
    log.debug("Getting Data from remotes")
    # data from pub source ie: naif, jaxa, esa, ect... 
    test_args_pub = [f"{mission_name}_naifKernels:/", "-l", "-R", "--format", "p", "--files-only"]
    test_data_pub = rclone(command="lsf", extra_args=test_args_pub, config=cfg_path, redirect_stdout=True, redirect_stderr=False)

    test_data_pub = test_data_pub.get('out').decode('utf-8')

    # data from S3 Bucket
    test_args_usgs = [f"{mission_name}_usgs:/", "-l","-R", "--format", "p", "--files-only"]
    test_data_usgs = rclone(command="lsf", extra_args=test_args_usgs, config=cfg_path, redirect_stdout=True, redirect_stderr=False)
    
    test_data_usgs = test_data_usgs.get('out').decode('utf-8')
    test_set = set()

    # need to change to compair data from remotes to aws isisdata dev-buckets
    for item in test_data_pub.split("\n"):
        if item == "" or item == " ":
            pass
        else:
            item = f"kernels/{item}"
            test_set.add(item)

    for item in test_data_usgs.split("\n"):
        test_set.add(item)

    diff = truth_set.difference(test_set)
    if len(diff) > 0:
        assert False    
    assert True