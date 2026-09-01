import sys
import xml.etree.ElementTree as ET
from pathlib import Path

import pytest

SCRIPTS = Path(__file__).resolve().parents[1] / "scripts"
sys.path.insert(0, str(SCRIPTS))

import build_llms_txt as bl

SRC_DIR = Path(__file__).resolve().parents[1] / "src"

FIXTURE = """<?xml version="1.0" encoding="UTF-8"?>
<application name="testapp">
  <brief>Does a test thing</brief>
  <description>
    <p>Projects a <def link="Cube">cube</def> using <i>spiceinit</i>.</p>
    <pre>
      Group = Mapping
        Foo = Bar
      EndGroup
    </pre>
    <table>
      <tr><td>PARAM</td><td>DEFAULT</td></tr>
      <tr><td>Radius</td><td>From PCK</td></tr>
    </table>
    <ul><li>first</li><li>second</li></ul>
  </description>
  <category>
    <categoryItem>Map Projection</categoryItem>
    <missionItem>Viking</missionItem>
  </category>
  <groups>
    <group name="Files">
      <parameter name="FROM">
        <type>cube</type>
        <fileMode>input</fileMode>
        <brief>Input cube</brief>
        <description>The input cube.</description>
      </parameter>
      <parameter name="MODE">
        <type>string</type>
        <default><item>CAMERA</item></default>
        <internalDefault>Computed</internalDefault>
        <list>
          <option value="CAMERA"><brief>From the cube</brief></option>
          <option value="MAP"><brief>From the map file</brief></option>
        </list>
      </parameter>
      <parameter name="RES">
        <type>double</type>
        <minimum inclusive="yes">0.0</minimum>
        <maximum inclusive="no">0.5</maximum>
        <greaterThan><item>MINRES</item></greaterThan>
      </parameter>
    </group>
  </groups>
  <examples>
    <example>
      <brief>Basic run</brief>
      <commandLine> from=in.cub
                    to=out.cub
      </commandLine>
    </example>
  </examples>
</application>
"""


@pytest.fixture
def app(tmp_path):
    path = tmp_path / "testapp" / "testapp.xml"
    path.parent.mkdir()
    path.write_text(FIXTURE)
    return bl.parse_app(path)


def test_to_markdown_paragraph_and_def():
    root = ET.fromstring("<d><p>A <def link='Cube'>cube</def> and <i>spiceinit</i>.</p></d>")
    assert bl.to_markdown(root) == "A cube and *spiceinit*."


def test_to_markdown_pre_is_fenced_and_dedented():
    root = ET.fromstring("<d><pre>\n    a\n      b\n</pre></d>")
    assert bl.to_markdown(root) == "```\na\n  b\n```"


def test_to_markdown_table_and_list():
    root = ET.fromstring(
        "<d><table><tr><td>A</td><td>B</td></tr><tr><td>1</td><td>2</td></tr></table>"
        "<ul><li>x</li><li>y</li></ul></d>")
    markdown = bl.to_markdown(root)
    assert "| A | B |" in markdown
    assert "| --- | --- |" in markdown
    assert "| 1 | 2 |" in markdown
    assert "- x\n- y" in markdown


def test_parse_app_metadata(app):
    assert app["name"] == "testapp"
    assert app["brief"] == "Does a test thing"
    assert app["categories"] == ["Map Projection"]
    assert app["missions"] == ["Viking"]
    assert app["examples"] == [("from=in.cub to=out.cub", "Basic run")]


def test_parse_app_parameters(app):
    params = {p["name"]: p for p in app["groups"][0]["parameters"]}
    assert params["FROM"]["type"] == "cube"
    assert params["FROM"]["file_mode"] == "input"
    assert params["MODE"]["default"] == "CAMERA"
    assert params["MODE"]["internal_default"] == "Computed"
    assert params["MODE"]["options"] == [("CAMERA", "From the cube"), ("MAP", "From the map file")]
    assert params["RES"]["constraints"] == [
        "Minimum: 0.0 (inclusive)", "Maximum: 0.5", "Greater than: MINRES"]


def test_render_llms_txt(app):
    text = bl.render_llms_txt([app], "9.9.9", "https://example.com")
    assert text.startswith("# ISIS 9.9.9")
    assert "\n> Integrated Software" in text
    assert ("- [testapp](https://example.com/9.9.9/Application/presentation/Tabbed/testapp/"
            "testapp.html): Does a test thing") in text
    assert "https://example.com/9.9.9/llms-full.txt" in text


def test_render_llms_full(app):
    text = bl.render_llms_full([app], "9.9.9", "https://example.com")
    assert "## testapp" in text
    assert "##### FROM" in text
    assert "- `CAMERA`: From the cube" in text
    assert "testapp from=in.cub to=out.cub" in text


def test_end_to_end(tmp_path):
    assert bl.main(["--src-dir", str(SRC_DIR), "--out-dir", str(tmp_path),
                    "--version", "dev"]) == 0

    index = (tmp_path / "llms.txt").read_text()
    full = (tmp_path / "llms-full.txt").read_text()

    assert index.startswith("# ISIS dev")
    assert index.count("\n- [") > 300
    assert "- [cam2map](" in index

    assert "## cam2map" in full
    assert "##### TO" in full
    assert "##### MAP" in full


def test_missing_src_dir_errors(tmp_path):
    with pytest.raises(SystemExit):
        bl.main(["--src-dir", str(tmp_path / "nope"), "--out-dir", str(tmp_path),
                 "--version", "dev"])
