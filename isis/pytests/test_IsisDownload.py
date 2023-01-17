
import sys
sys.path.append('../scripts/')

import os
import pytest 
from unittest import mock
from tempfile import TemporaryDirectory
from pathlib import Path, exists

from importlib.util import spec_from_loader, module_from_spec
from importlib.machinery import SourceFileLoader 

spec = spec_from_loader("downloadIsisData", SourceFileLoader("downloadIsisData", "../scripts/downloadIsisData"))
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

        return stdout, stderr


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


def test_create_rclone_args():
    with TemporaryDirectory() as tdir: 
        dest = Path(tdir)
        args = did.create_rclone_arguments(str(dest), "lro_naifKernels:", ntransfers=100, rclone_kwargs=["--dry_run"])
        assert args == ['lro_naifKernels:', str(dest/"lro"/"kernels"), '--progress', '--checkers=100', '--transfers=100', '--track-renames', '--log-level=WARNING', '--dry_run']


def test_file_filtering():
    with TemporaryDirectory() as tdir:
        dest = Path(tdir)
        # Run the rclone command to download files
        did.rclone("lsf", "test", dest, extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)
        # Check if the specified files are not present in the destination directory
        assert not (dest / "a_older_versions").exists()
        assert not (dest / "kernels" / "*" / "former_versions").exists()
        assert not (dest / "corrupt_files").exists()
        assert not (dest / "zzarchive").exists()
        assert not (dest / "kernels" / "*" / "original").exists()
        assert not (dest / "gallileo" / "kernels" / "ck" / "prime_mission" / "*" / "extended_mission").exists()
        assert not (dest / "gallileo" / "kernels" / "*" / "prime_mission").exists()
        assert not (dest / "galileo" / "kernels" / "ck" / "GEM").exists()
        assert not (dest / "clementine1" / "kernels" / "ck" / "save").exists()
        assert not (dest / "chandrayaan1" / "kernels" / "spk" / "SAVE_SCS_2017-11-22").exists()
        assert not (dest / "*" / "kernels" / "fk" / "release.*" / "*").exists()
        assert not (dest / "cassini" / "kernels" / "fk" / "Archive").exists()
