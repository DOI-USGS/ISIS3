---
name: isis-review
description: Perform a read-only ISIS3 PR-readiness review, especially for legacy Makefile-to-gtest/CTest conversions. Check build-system integration, fixtures and data, scientific assertions, reviewer-comment resolution, and git hygiene.
disable-model-invocation: true
---

You are reviewing the current ISIS3 branch for PR readiness.

Default mode: read-only.

Do not edit files, stage files, commit, push, or create/update PRs during this command.

If changes are needed, report them as recommendations only. Wait for a separate explicit follow-up request before making edits.

Run or inspect:
- git status --short
- git diff --check
- git diff --stat
- git diff
- relevant CTest output if available
- changed change-note files

Reviewer-comment reconciliation:

- If reviewer comments are provided or visible in context, map each comment to its current resolution:
  - addressed
  - partially addressed
  - not addressed
  - intentionally declined with rationale
- Do not declare the branch ready if any reviewer-requested change is unresolved without a clear rationale.

Known ISIS3 reviewer preferences to check explicitly:

- Prefer ISIS framework utilities over ad hoc Qt/local helpers when available.
  - For list files, check whether ISIS FileList can replace QFile/QTextStream list-writing helpers.
  - For generated test cubes, check whether an existing CubeFixtures helper can be reused or whether a new shared fixture belongs in CubeFixtures.cpp/.h.

- For DN-manipulation tests, require aggregate output validation.
  - Prefer Statistics or Histogram checks for min/max/average/sum/valid-pixel counts where appropriate.
  - Single-pixel checks may supplement but should not be the only DN validation.

- For reusable test assertion helpers, prefer shared utilities.
  - If a local helper validates common cube statistics, histogram values, PVL labels, dimensions, or DN patterns, check whether it belongs in TestUtilities.cpp/.h, CubeFixtures.cpp/.h, or another shared test utility.
  - Local helpers are acceptable when they are app-specific, very small, or encode behavior unique to this app.
  - Flag local helpers that are likely to be copied across future app conversions.

- For callable app conversions, check production-scope hygiene.
  - Internal helpers, calculators, and free functions in app .cpp files should be static or inside an anonymous namespace unless intentionally exported.
  - Tests should pass UserInterface explicitly and avoid global Application::GetUserInterface() lookups where possible.
  - If production app files changed, verify the change is necessary for callable app testing and is minimal.
  - Check that new app entry points follow existing ISIS callable app patterns: appName(UserInterface &ui), header declaration, and main.cpp delegation.
  - Check that production helpers are not accidentally exposed through headers unless required.

- For test style, flag unnecessary thin wrappers.
  - If a helper only constructs UserInterface and calls the app once, prefer inlining it in the test unless it significantly improves readability or removes meaningful duplication.

- For disabled tests, challenge them.
  - Do not keep disabled camera/data-heavy tests merely to preserve legacy coverage.
  - Remove, rewrite with synthetic/minimal data, or clearly justify them.

- For branch/process hygiene:
  - If the branch already has reviewer activity, warn before recommending force-push. Prefer normal push unless history cleanup is explicitly needed.
  - Do not declare ready unless the verdict cites concrete evidence: clean status scope, clean whitespace check, relevant tests or known blocker, and CI status if available.

- For converted gtests, flag subprocess execution by default.
  - Tests should call the app function directly with `UserInterface ui(APP_XML, args); appname(ui);`.
  - Flag `QProcess`, shelling out to `$ISISROOT/bin/APPNAME`, CLI wrappers, or helper functions that only hide subprocess execution.
  - Subprocess-based gtests require explicit justification based on a diagnosed in-process failure, not convenience or legacy parity.

Delegate the review into these independent passes:

1. Build-system reviewer
   - Check that the new test lives under isis/tests.
   - Check that legacy app tsts files are fully removed when appropriate.
   - Check for stale CTest/Makefile test risk.
   - Check path handling, especially _SOURCE_PREFIX and committed test data.
   - Check for `QProcess`, `process.start`, `$ISISROOT/bin/APPNAME`, shell execution, or other subprocess patterns in converted gtests.

2. Fixture/data reviewer
   - Check whether synthetic cubes replace external ISISTESTDATA usage.
   - Verify any data-footprint claim against actual removed files.
   - Flag unnecessary helpers or duplicated fixture logic.

3. Assertion/science reviewer
   - Check that tests verify meaningful app outputs.
   - Look for labels, mapping groups, histograms, DN values, and output cube properties.
   - Flag tests that only prove the app ran.
   - Flag broadly reusable assertion helpers that should be moved to shared test utilities instead of remaining local to one FunctionalTests file.

4. PR hygiene reviewer
   - Check changed files are limited to intended scope.
   - Check no .claude, build artifacts, or unrelated files are present.
   - Check changelog wording matches implementation.
   - Check whether production-file changes are minimal, justified, and scoped to the conversion.

End with:
- Verdict: ready / not ready / needs small fix
- Blocking issues
- Non-blocking suggestions
- Reviewer-comment resolution map, if applicable
- Exact files that should be committed
- Exact commands the user should run next
