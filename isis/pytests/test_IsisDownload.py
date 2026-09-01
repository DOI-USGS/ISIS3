
import os
import shutil
import subprocess
import sys
import argparse
import pytest
from unittest import mock
from tempfile import TemporaryDirectory
from pathlib import Path
import tempfile

from importlib.util import spec_from_loader, module_from_spec
from importlib.machinery import SourceFileLoader

SCRIPT = Path(__file__).resolve().parent.parent / "scripts" / "downloadIsisData"
sys.path.append(str(SCRIPT.parent))

spec = spec_from_loader("downloadIsisData", SourceFileLoader("downloadIsisData", str(SCRIPT)))
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


def make_args(**kwargs):
    defaults = dict(filter=None, include=None, exclude=None, no_kernels=False, num_transfers=10)
    defaults.update(kwargs)
    ns = argparse.Namespace(**defaults)
    did.args = ns
    return ns


def filter_rules(rclone_args):
    return [a[len('--filter='):] for a in rclone_args if a.startswith('--filter=')]


def test_rclone():
    with mock.patch("subprocess.Popen", MockedPopen):
        res = did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)
        assert res["out"].decode() == "Success"


def test_rclone_unknown_exception():
    with mock.patch("subprocess.Popen", MockedBustedPopen):
        with pytest.raises(Exception, match="idk"):
            did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only"], redirect_stdout=True, redirect_stderr=True)


def test_rclone_with_auth():
    # Test the rclone function when auth is required
    with mock.patch("subprocess.Popen", MockedPopen):
        res = did.rclone("lsf", "test", extra_args=["-l", "-R", "--format", "p", "--files-only", "--rc-web-gui", "user:pass"], redirect_stdout=True, redirect_stderr=True)
        assert res["out"].decode() == "Success"
        assert '--rc-web-gui' in res['args']


@pytest.mark.parametrize("mission, expected", [
    ("isistestdata", did.TESTDATA_FILTERS),
    ("base", did.CURATED_FILTERS),
    ("legacybase", did.CURATED_FILTERS),
    ("control", did.CURATED_FILTERS),
    ("mro", did.KERNEL_FILTERS),
])
def test_filters_for_mapping(mission, expected):
    assert did.filters_for(mission) == expected


def test_filters_for_returns_a_copy():
    rules = did.filters_for("mro")
    rules.append("- poison/**")
    assert "- poison/**" not in did.KERNEL_FILTERS


def test_testdata_filters_keep_comparable_extensions():
    rules = did.filters_for("isistestdata")
    assert not [r for r in rules if r.endswith(('*.txt', '*.csv', '*.lbl'))]


def test_filters_are_idempotent():
    ns = make_args(exclude=['foo/**'])
    first = did.create_rclone_arguments('/tmp/dest', 'mro', ns)
    second = did.create_rclone_arguments('/tmp/dest', 'mro', ns)
    assert first == second


def test_filter_flag_does_not_raise():
    ns = make_args(filter=['foo/**'])
    rules = filter_rules(did.create_rclone_arguments('/tmp/dest', 'mro', ns))
    assert '- foo/**' in rules


def test_exclude_and_no_kernels_flags():
    ns = make_args(exclude=['foo/**'], no_kernels=True)
    rules = filter_rules(did.create_rclone_arguments('/tmp/dest', 'mro', ns))
    assert '- foo/**' in rules
    assert '- ck/**' in rules


def test_include_appends_catchall_once():
    ns = make_args(include=['ck/**'])
    for _ in range(2):
        rules = filter_rules(did.create_rclone_arguments('/tmp/dest', 'mro', ns))
        assert rules.count('- *') == 1
        assert rules[-1] == '- *'
        assert '+ ck/**' in rules


FIXTURE_FILES = [
    "isis/src/base/apps/spicefit/tsts/default/truth/sunposition.txt",
    "isis/src/clementine/apps/clemuvviscal/tsts/filters/input/source/lua1841h.283",
    "isis/src/base/unitTestData/SpectralDefinition2D/calibration-test.csv",
    "isis/src/apollo/unitTestData/AS15-M-1450.lbl",
    "kernels/ek/ekinfo.txt",
    "kernels/ck/prime_mission/x.bc",
    "kernels/spk/de430.bsp",
    "kernelTesting/N1477312678_2.lbl",
    "print.prt",
]


@pytest.fixture(scope="module")
def fixture_tree(tmp_path_factory):
    root = tmp_path_factory.mktemp("isisdata")
    for rel in FIXTURE_FILES:
        target = root / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        target.touch()
    return root


def surviving(rules, root):
    args = ["rclone", "lsf", "-R", "--files-only"] + [f"--filter={r}" for r in rules] + [str(root)]
    out = subprocess.run(args, capture_output=True, text=True, check=True).stdout
    return set(out.split())


@pytest.mark.skipif(shutil.which("rclone") is None, reason="rclone not installed")
@pytest.mark.parametrize("mission, kept, dropped", [
    ("isistestdata",
     ["isis/src/base/apps/spicefit/tsts/default/truth/sunposition.txt",
      "isis/src/clementine/apps/clemuvviscal/tsts/filters/input/source/lua1841h.283",
      "isis/src/base/unitTestData/SpectralDefinition2D/calibration-test.csv",
      "isis/src/apollo/unitTestData/AS15-M-1450.lbl"],
     ["print.prt"]),
    ("mro",
     ["kernels/spk/de430.bsp"],
     ["kernels/ek/ekinfo.txt",
      "kernels/ck/prime_mission/x.bc",
      "isis/src/apollo/unitTestData/AS15-M-1450.lbl"]),
    ("base",
     ["kernelTesting/N1477312678_2.lbl",
      "kernels/spk/de430.bsp"],
     []),
])
def test_filter_behavior(fixture_tree, mission, kept, dropped):
    files = surviving(did.filters_for(mission), fixture_tree)
    for path in kept:
        assert path in files
    for path in dropped:
        assert path not in files
