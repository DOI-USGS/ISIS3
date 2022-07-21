
import sys
sys.path.append('../scripts/')

import os
import pytest 
from unittest import mock
from tempfile import TemporaryDirectory
from pathlib import Path

from downloadIsisData import rclone, create_rclone_arguments

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

        return stdout, stderr


class MockedBustedPopen:
    def __init__(self, args, **kwargs):
        raise Exception("idk what happened")

@mock.patch('downloadIsisData.which', return_value=False)
def test_rclone_not_installed(mock_which):
    with pytest.raises(ProcessLookupError, match="rclone is not installed"):
        rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)


@mock.patch("subprocess.Popen", MockedPopen)
def test_rclone():
    res = rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)
    assert res["out"].decode() == "Success"

@mock.patch("subprocess.Popen", MockedBustedPopen)
def test_rclone_unknown_exception():
    with pytest.raises(Exception, match="idk"):
        res = rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)


def test_create_rclone_args():
    with TemporaryDirectory() as tdir: 
        dest = Path(tdir) / "test"
        args = create_rclone_arguments(str(dest), "lro_naifKernels:", dry_run=False, ntransfers=100)
        assert args == ['lro_naifKernels:', str(dest/"lro"/"kernels"), '--progress', '--checkers=100', '--transfers=100', '--track-renames', '--log-level=WARNING']
        assert dest.exists()


