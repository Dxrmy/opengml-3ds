# Master Plan: Comprehensive Architectural Audit & Port Review

This plan details the exhaustive steps to verify the OpenGML 3DS Port (`opengml_repo`) against the original codebase (`opengml-master`) and confirm all intended features, fixes, and architectural decisions are sound.

## Phase 1: Git Merge Forensic Review
- [ ] Analyze the git history of `opengml_repo` to identify all recent merges.
- [ ] Verify that critical security fixes (buffer overflows, command injection) from earlier PRs have not been accidentally reverted by subsequent refactors.
- [ ] Confirm that 3DS-specific optimizations (Morton tiling, VBO updates) are intact.

## Phase 2: Original vs. Port Architectural Comparison
- [ ] **Loader Module:** Compare `src/project/DataWinLoader.cpp` (if present) or `src/project/Project.cpp` to ensure binary data loading logic aligns with the original intent but correctly integrates 3DS limitations.
- [ ] **VM & Executor:** Compare `src/interpreter/execute.cpp` and `src/interpreter/Executor.cpp`. Verify that bytecode handling, stack management, and GC mechanisms are robust and haven't lost original safety checks.
- [ ] **Display/Rendering:** Contrast the original OpenGL rendering in `src/interpreter/display/Display.cpp` with the 3DS Citro3D implementation in `src/interpreter/display/Display_3ds.cpp`. Verify blend modes, texture caching, and depth buffer handling.
- [ ] **Input & File System:** Check the 3DS specific input mappings in `Display_3ds.cpp` and the sandboxing logic in `src/interpreter/Filesystem.cpp` against original cross-platform implementations.

## Phase 3: Build & Test Verification
- [ ] Execute the build scripts (e.g., `make -f Makefile.3ds`) to ensure the codebase still compiles successfully for the target platform.
- [ ] Run the headless VM test suite (`test/tHeadlessVM.cpp`) and any asset injection tests (`test/tAssetInjection.cpp`) to verify core logic execution.
- [ ] Execute `validate_win.py` against the Undertale `data.win` to confirm binary parsing integrity.

## Phase 4: Unimplemented Feature & Bug Hunt
- [ ] Scan the entire `opengml_repo` codebase for `TODO`, `FIXME`, `NYI` (Not Yet Implemented), and `STUB` markers.
- [ ] Identify any missing standard library functions (`ogm::interpreter::fn::*`) required by Undertale that are currently empty or return default values.
- [ ] Check for hardcoded limits (e.g., maximum textures, memory boundaries) that might pose a risk on 3DS hardware.

## Phase 5: Final Reporting & Estimation
- [ ] Compile all findings into `docs/COMPREHENSIVE_AUDIT.md`.
- [ ] Detail every discrepancy between the original and the port.
- [ ] List all remaining errors, failing tests, and unimplemented features.
- [ ] Provide a final "Percentage to Completion" estimate and a timeframe for finishing the port based on the remaining work.
