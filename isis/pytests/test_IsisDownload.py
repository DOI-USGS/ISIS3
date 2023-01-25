
import sys
sys.path.append('../scripts/')

import os
import pytest 
from unittest import mock
from tempfile import TemporaryDirectory
from pathlib import Path
import tempfile

from importlib.util import spec_from_loader, module_from_spec
from importlib.machinery import SourceFileLoader

spec = spec_from_loader("downloadIsisData", SourceFileLoader("downloadIsisData", "isis/scripts/downloadIsisData"))
downloadIsisData = module_from_spec(spec)
spec.loader.exec_module(downloadIsisData)
did = downloadIsisData


class MockedPopen:
    def __init__(self, args, **kwargs):
        self.args = args
        self.returncode = 0

    def __enter__(self):
        return self

    def __exit__(self, exc_type, value, traceback):
        pass

    def communicate(self, input=None, timeout=None):
        if self.args[0] == 'rclone':
            stdout = "Success".encode("utf-8")
            stderr = ''.encode("utf-8")
            self.returncode = 0
        else:
            raise Exception()

        return {'out': stdout, 'stderr': stderr, 'args': self.args, 'returncode': self.returncode}


class MockedBustedPopen:
    def __init__(self, args, **kwargs):
        raise Exception("idk what happened")


@mock.patch("subprocess.Popen", MockedPopen)
def test_rclone():
    res = did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)
    assert res["out"].decode() == "Success"

@mock.patch("subprocess.Popen", MockedBustedPopen)
def test_rclone_unknown_exception():
    with pytest.raises(Exception, match="idk"):
        res = did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)


def test_rclone():
    with mock.patch("subprocess.Popen", MockedPopen):
        res = did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)
        assert res["out"].decode() == "Success"


def test_rclone_unknown_exception():
    with mock.patch("subprocess.Popen", MockedBustedPopen):
        with pytest.raises(Exception, match="idk"):
            did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)


def test_create_rclone_args():
    with TemporaryDirectory() as tdir: 
        dest = Path(tdir)
        args = did.create_rclone_arguments(str(dest), "lro_naifKernels:", ntransfers=100, rclone_kwargs=["--dry_run"])
        assert args == ['lro_naifKernels:', str(dest/"lro"/"kernels"),
                        '--progress', '--checkers=100', '--transfers=100',
                        '--track-renames', '--log-level=WARNING',"--filter",
                        f"{did.exclude_string}", '--dry_run']


def test_file_filtering():
    dest = tempfile.mkdtemp()  # Create a temporary directory and assign the path to 'dest'

    # Get the rclone command that was passed to subprocess.run in downloadIsisData
    rclone_args = did.create_rclone_arguments(dest, "lro_naifKernels:", ntransfers=100, rclone_kwargs=["--dry_run"])

    # Check if the command contains the --exclude argument
    assert "--filter" in rclone_args

    # Check if the command contains the specified regexps
    for file in did.exclude_string:
        assert file in did.exclude_string


def test_create_rclone_arguments_with_include():
    destination = "/path/to/destination"
    mission_name = "mission_name:name"
    ntransfers = 10
    rclone_kwargs = ["--include=pattern1"]
    expected_args = ["mission_name:name", "/path/to/destination/mission", "--progress", "--checkers=10", "--transfers=10", "--track-renames", "--log-level=WARNING", "--filter", f"{did.exclude_string},pattern1+","--include=pattern1"]
    assert did.create_rclone_arguments(destination, mission_name, ntransfers, rclone_kwargs) == expected_args


def test_rclone_with_auth():
    # Test the rclone function when auth is required
    with mock.patch("subprocess.Popen", MockedPopen):
        res = did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only", "--rc-web-gui", "user:pass"], redirect_stdout=True, redirect_stderr=True)
        assert res["out"].decode() == "Success"
        assert '--rc-web-gui' in res['args']


def test_create_rclone_args_with_no_kwargs():
    # Test the create_rclone_arguments function when no kwargs are passed
    with TemporaryDirectory() as tdir:
        dest = Path(tdir)
        args = did.create_rclone_arguments(str(dest), "lro_naifKernels:", ntransfers=100)
        assert args == ['lro_naifKernels:', str(dest/"lro"/"kernels"),
                        '--progress', '--checkers=100', '--transfers=100',
                        '--track-renames', '--log-level=WARNING',"--filter",
                        f"{did.exclude_string}"]


def test_file_filtering_with_hidden_files():
    # Test the file_filtering function when hidden files are present
    dest = tempfile.mkdtemp()  # Create a temporary directory and assign the path to 'dest'
    Path(dest+"/.hidden_file").touch()
    # Get the rclone command that was passed to subprocess.run in downloadIsisData
    rclone_args = did.create_rclone_arguments(dest, "lro_naifKernels:", ntransfers=100, rclone_kwargs=["--dry_run"])
    # Check if the command contains the --exclude argument
    assert "--filter" in rclone_args
    # Check if the command contains the specified regexps
    for file in did.exclude_string:
        assert file in did.exclude_string
    # Check if the hidden file is excluded
    assert "hidden_file" in did.exclude_string
