#!/usr/bin/env python3
"""
Generate CITATION.cff and citation blocks for ISIS releases.

Usage:
    python generate_citation.py --version 10.0.0 --doi 10.5066/PXXXXXXX
    python generate_citation.py --version 10.0.0 --doi 10.5066/PXXXXXXX --date 2024-12-15
"""

import argparse
import json
import yaml
from datetime import datetime
from pathlib import Path


class CitationGenerator:
    def __init__(self, version, doi, release_date=None):
        self.version = version
        self.doi = doi
        self.release_date = release_date or datetime.now().strftime("%Y-%m-%d")
        self.repo_root = Path(__file__).parent.parent.parent

    def get_default_authors(self):
        """Get default author information"""
        return [{
            'name': 'USGS Astrogeology Science Center',
            'email': 'krodriguez@usgs.gov'
        }]

    def generate_cff(self):
        """Generate CITATION.cff content"""
        authors = self.get_default_authors()

        cff_data = {
            'cff-version': '1.2.0',
            'message': 'If you use ISIS in your research, please cite it as below.',
            'title': 'Integrated Software for Imagers and Spectrometers (ISIS)',
            'version': self.version,
            'date-released': self.release_date,
            'doi': self.doi,
            'repository-code': 'https://github.com/DOI-USGS/ISIS3',
            'url': 'https://isis.astrogeology.usgs.gov',
            'license': 'CC0-1.0',
            'type': 'software',
            'authors': authors,
            'keywords': [
                'planetary science',
                'image processing',
                'photogrammetry',
                'cartography',
                'remote sensing'
            ],
            'abstract': (
                'ISIS (Integrated Software for Imagers and Spectrometers) is a '
                'specialized software package developed by the USGS for processing '
                'and analyzing planetary remote sensing data.'
            )
        }

        return yaml.dump(cff_data, sort_keys=False, allow_unicode=True)

    def generate_bibtex(self):
        """Generate BibTeX citation"""
        year = self.release_date.split('-')[0]
        version_key = self.version.replace('.', '')

        bibtex = f"""@software{{ISIS{version_key},
  author = {{{{USGS Astrogeology Science Center}}}},
  title = {{{{Integrated Software for Imagers and Spectrometers (ISIS) version {self.version}}}}},
  year = {{{year}}},
  publisher = {{{{U.S. Geological Survey}}}},
  version = {{{self.version}}},
  doi = {{{self.doi}}},
  url = {{{{https://doi.org/{self.doi}}}}}
}}"""
        return bibtex

    def generate_apa(self):
        """Generate APA citation"""
        year = self.release_date.split('-')[0]

        apa = (
            f"USGS Astrogeology Science Center. ({year}). "
            f"Integrated Software for Imagers and Spectrometers (ISIS) "
            f"(Version {self.version}) [Computer software]. U.S. Geological Survey. "
            f"https://doi.org/{self.doi}"
        )
        return apa

    def generate_readme_block(self):
        """Generate Markdown citation block for README"""
        block = f"""## Citing ISIS

If you use ISIS in your research, please cite the specific version you used:

### Latest Release (ISIS {self.version})

**APA Format:**
```
{self.generate_apa()}
```

**BibTeX Format:**
```bibtex
{self.generate_bibtex()}
```

Use ["Cite this repository"](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/about-citation-files) button on dev branch for citation information for dev builds. 

### Citing Previous Versions

To cite a specific earlier version, visit our [GitHub Releases](https://github.com/DOI-USGS/ISIS3/releases) page and click on the version you used. Each release has a ["Cite this repository"](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/about-citation-files) button with version-specific citation formats.

### Machine-Readable Citation

For automated citation tools, see our [CITATION.cff](CITATION.cff) file. This file is automatically recognized by GitHub, Zotero, and other citation managers.

### Questions?

If you have questions about citing ISIS, please [open an issue](https://github.com/DOI-USGS/ISIS3/issues/new).
"""
        return block


    def write_cff_file(self, output_path=None):
        """Write CITATION.cff to file"""
        if not output_path:
            output_path = self.repo_root / "CITATION.cff"

        cff_content = self.generate_cff()

        with open(output_path, 'w') as f:
            f.write(cff_content)

        print(f"✓ Generated {output_path}")
        return output_path

    def write_readme_citation(self, readme_path=None):
        """Update README with citation block"""
        if not readme_path:
            readme_path = self.repo_root / "README.md"

        with open(readme_path, 'r') as f:
            readme_content = f.read()

        citation_block = self.generate_readme_block()

        # Find citation section or insert before License
        import re
        if "## Citing ISIS" in readme_content:
            # Replace existing citation section
            pattern = r'## Citing ISIS.*?(?=\n## |\Z)'
            readme_content = re.sub(
                pattern,
                citation_block.rstrip(),
                readme_content,
                flags=re.DOTALL
            )
        else:
            # Insert before License section or at end
            if "## License" in readme_content:
                readme_content = readme_content.replace(
                    "## License",
                    f"{citation_block}\n## License"
                )
            else:
                # Append at end
                readme_content = readme_content.rstrip() + f"\n\n{citation_block}"

        with open(readme_path, 'w') as f:
            f.write(readme_content)

        print(f"✓ Updated {readme_path}")
        return readme_path


def main():
    parser = argparse.ArgumentParser(
        description='Generate ISIS citation files',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python generate_citation.py --version 10.0.0 --doi 10.5066/P13TADS5
  python generate_citation.py --version 10.0.0 --doi 10.5066/P13TADS5 --date 2024-12-15
        """
    )
    parser.add_argument(
        '--version',
        required=True,
        help='ISIS version (e.g., 10.0.0)'
    )
    parser.add_argument(
        '--doi',
        required=True,
        help='USGS DOI (e.g., 10.5066/P13TADS5)'
    )
    parser.add_argument(
        '--date',
        help='Release date (YYYY-MM-DD, defaults to today)'
    )
    parser.add_argument(
        '--cff-only',
        action='store_true',
        help='Only generate CITATION.cff'
    )
    parser.add_argument(
        '--readme-only',
        action='store_true',
        help='Only update README'
    )

    args = parser.parse_args()

    generator = CitationGenerator(
        version=args.version,
        doi=args.doi,
        release_date=args.date
    )

    print(f"\nGenerating citations for ISIS {args.version}")
    print(f"DOI: {args.doi}")
    print(f"Date: {generator.release_date}\n")

    if not args.readme_only:
        generator.write_cff_file()

    if not args.cff_only:
        generator.write_readme_citation()

    print("\n✓ Citation generation complete!")
    print(f"\nFiles updated:")
    if not args.readme_only:
        print(f"  - CITATION.cff")
    if not args.cff_only:
        print(f"  - README.md")
    print(f"\nNext steps:")
    print(f"  1. Review changes: git diff")
    print(f"  2. Validate CITATION.cff at https://citation-file-format.github.io/")
    print(f"  3. Commit: git add CITATION.cff README.md")


if __name__ == '__main__':
    main()
