# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

AzerothCore is an open-source MMORPG server emulator for World of Warcraft patch 3.3.5a (Wrath of the Lich King). It's a C++20 project built with CMake, using MySQL for data storage. Licensed under GNU GPL v2.

This checkout is a fork of `azerothcore/azerothcore-wotlk` (remote `Lloyd2k4/azerothcore-wotlk`). The development branch is `azerothcore-LMU`; `master` mirrors upstream. Modules under `modules/` (currently `mod-ale`) are compiled into the worldserver after core.

## Build Commands

- **Do not build unless explicitly requested** — a full build takes a long time.
- Out-of-source builds are mandatory; the build never writes into the source tree.

### Preferred: the `acore.sh` dashboard

`./acore.sh` is the project's own tooling (used by CI and the `.vscode` tasks). On Windows run it from Git Bash/MSYS.

```bash
./acore.sh init             # first-time setup
./acore.sh install-deps     # OS dependencies
./acore.sh compiler build   # configure + compile + install
./acore.sh compiler clean
./acore.sh run-worldserver  # restarter loop
./acore.sh run-authserver
./acore.sh setup-db
./acore.sh module install <mod-name>
```

Paths from `conf/dist/config.sh`: build tree `var/build/obj`, install tree `env/dist` (on Linux binaries in `env/dist/bin`), client data `env/dist/data`; configs are installed to `env/dist/etc` on Linux/macOS and `env/dist/configs` on Windows. Override via `conf/config.sh` or env vars (`BUILDPATH`, `CTYPE`, `CBUILD_TESTING`, `CCUSTOMOPTIONS`, `AC_CCACHE`, ...). On MSYS it expects MySQL at `C:\tools\mysql\current`.

### Manual CMake

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/azeroth-server -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DSCRIPTS=static -DMODULES=static
make -j$(nproc)
make install
```

- `CMAKE_BUILD_TYPE` defaults to `RelWithDebInfo` when unset.
- On Windows this checkout is opened through the Visual Studio/Rider CMake integration: build tree `out/build/x64-Debug` (Ninja generator), install tree `out/install/x64-Debug`. Its `CMakeCache.txt` currently has `BUILD_TESTING=OFF`.

### Key CMake options

- `SCRIPTS`: none, static, dynamic, minimal-static, minimal-dynamic (default: static; `minimal` only builds the whitelisted Commands and Spells modules)
- `MODULES`: none, static, dynamic (default: static)
- `APPS_BUILD`: none, all, auth-only, world-only (default: all)
- `TOOLS_BUILD`: none, all, db-only, maps-only (default: none)
- `BUILD_TESTING`: build unit tests (default: OFF)
- `USE_COREPCH` / `USE_SCRIPTPCH`: precompiled headers (default: ON); `-DNOPCH=1` disables both
- `WITH_WARNINGS`, `WITH_COREDEBUG`: warnings and extra core debug code (CI builds with `-Werror`)

## Unit Tests

Tests use Google Test/gmock and live in `src/test/`, built into the `unit_tests` target which links `game`, `gtest_main`, `gmock_main`. They are only built when `BUILD_TESTING=ON` **and** the worldserver is enabled (`BUILD_APPLICATION_WORLDSERVER`), so configure with `-DBUILD_TESTING=ON -DAPPS_BUILD=all` (or `CBUILD_TESTING=ON` for acore.sh).

```bash
cmake .. -DBUILD_TESTING=ON -DAPPS_BUILD=all && make -j$(nproc)

./src/test/unit_tests                                  # all tests
./src/test/unit_tests --gtest_filter=FormulasTest.*    # one test/fixture
ctest -R unit                                          # via CTest
```

On the Visual Studio build the binary is `out/build/x64-Debug/src/test/unit_tests.exe`; with acore.sh it is `var/build/obj/src/test/unit_tests`.

- `src/test/mocks/WorldMock.h` is a gmock `IWorld`; tests swap it in with `sWorld.reset(new NiceMock<WorldMock>())` and restore the previous world in `TearDown`.
- Module tests are appended to `unit_tests` via the `ACORE_MODULE_TEST_SOURCES` global CMake property.
- `./acore.sh test` runs the in-game/bash test framework, **not** this Google Test suite.

## Architecture

### Two server executables
- **authserver** (`src/server/apps/authserver/`): authentication and realm selection (port 3724)
- **worldserver** (`src/server/apps/worldserver/`): all gameplay (port 8085)

### Source layout (`src/`)

- **`src/common/`** - shared libraries: networking (Asio), cryptography, configuration, logging, threading, collision detection, utilities
- **`src/server/game/`** - core game logic (~50 subsystems), the heart of the worldserver
- **`src/server/scripts/`** - content scripts (bosses, spells, commands, instances)
- **`src/server/database/`** - database abstraction layer and schema updater
- **`src/server/shared/`** - code shared between auth and world servers (packets, network, realm definitions)
- **`src/test/`** - unit tests (Google Test)
- **`src/genrev/`** - generates `revision.h`; the build embeds git revision info

### Key game subsystems (`src/server/game/`)

- **Entities/** - core game objects: `Player`, `Creature`, `Unit`, `Item`, `GameObject`
- **Spells/** - spell mechanics, aura system, spell effects
- **Maps/** - map management, grid system, instancing
- **Handlers/** - client packet handlers, one file per system (`MovementHandler.cpp`, `SpellHandler.cpp`, ...); these are `WorldSession` methods
- **AI/** - creature AI framework
- **Scripting/** - script system with typed base classes (`ScriptObject` subclasses: `CreatureScript`, `SpellScript`, `InstanceMapScript`, `GameObjectScript`, `CommandScript`, etc.)
- **Server/** - `WorldSession` (per-player connection), `World` (global state), opcode definitions

### Scripting system

Scripts are grouped into modules by directory under `src/server/scripts/`: `Commands`, `Custom`, `EasternKingdoms`, `Events`, `Kalimdor`, `Northrend`, `OutdoorPvP`, `Outland`, `Pet`, `Spells`, `World`.

1. Define a class inheriting from `SpellScript`, `CreatureScript`, etc.
2. Implement an `AddSC_<name>()` function that calls `RegisterSpellScript(ClassName)` (or the matching registration macro).
3. Declare and call that `AddSC_*` in the module's loader file — e.g. `Spells/spells_script_loader.cpp`, `Northrend/northrend_script_loader.cpp`. The loader defines `void Add<Directory>Scripts()` (name must match the directory, e.g. `AddSpellsScripts()`); skipping this step silently drops the script.
4. The top-level loader (generated from `ScriptLoader.cpp.in.cmake`) calls each `Add<Directory>Scripts()`.

Source files are glob-collected (`CollectSourceFiles`), so adding a `.cpp` needs no CMake edit — just re-run CMake. Spell scripts are split per class (`spell_mage.cpp`, `spell_dk.cpp`, `spell_generic.cpp`, ...).

### Databases

- **acore_auth** - accounts, realm list, bans (`data/sql/base/db_auth/`)
- **acore_characters** - character data, inventories, progress (`data/sql/base/db_characters/`)
- **acore_world** - game content: creatures, items, quests, spells, loot (`data/sql/base/db_world/`)

The servers apply SQL updates at startup. Each database's `updates_include` table maps directories to states: `PENDING` → `data/sql/updates/pending_db_*`, `RELEASED` → `data/sql/updates/db_*`, `ARCHIVED` → `data/sql/archive/db_*`, `CUSTOM` → `data/sql/custom/db_*`.

- New SQL goes in `data/sql/updates/pending_db_*`; filenames must be unique/random (`create_sql.sh` there creates `rev_<timestamp>.sql`).
- Never edit `data/sql/base/` files or already-merged files in `data/sql/updates/db_*`; corrections are new pending updates.

### Module system

External modules are loaded from `modules/`; each is a subdirectory with its own `CMakeLists.txt` and is built after core. Disable specific modules with `-DDISABLED_AC_MODULES="mod1;mod2"`; manage them with `./acore.sh module`. Skeleton: https://github.com/azerothcore/skeleton-module/

### Configuration

Runtime config templates are `src/server/apps/authserver/authserver.conf.dist` and `src/server/apps/worldserver/worldserver.conf.dist`; they are installed next to the binaries and copied to `*.conf` by the installer (only when the `.conf` doesn't exist yet). Edit source `.conf.dist` only when adding a new option.

### Dependencies

Bundled in `deps/`: boost, MySQL client, OpenSSL, zlib, recastnavigation (pathfinding), g3dlite (geometry), fmt, argon2, jemalloc, and others.

## Code Style Checks

CI runs these; run them locally before finishing:

```bash
python apps/codestyle/codestyle-cpp.py   # from repo root
python apps/codestyle/codestyle-sql.py
cppcheck --force --inline-suppr --suppressions-list=./.suppress.cppcheck src/
```

Style rules (`.editorconfig`):

- 4-space indentation for C++ (no tabs); 2-space for JSON/YAML/shell
- UTF-8, LF line endings, final newline, no trailing whitespace
- Max 80 character line length
- No braces around single-line statements
- Use `{}` to parse variables into output instead of `%u` etc.

## Commit Message Format

Conventional Commits (see `.git_commit_template.txt`):

```
Type(Scope/Subscope): Short description (max 50 chars)
```

- **Types**: feat, fix, refactor, style, docs, test, chore
- **Scopes**: Core (C++ changes), DB (SQL changes)
- Examples: `fix(Core/Spells): Fix damage calculation for Fireball`, `fix(DB/SAI): Missing spell to NPC Hogger`
- Imperative mood, capitalized, no trailing period; body lines max 72 chars and explain why

## PR Requirements

- AI tool usage must be disclosed in PRs
- In-game testing expected
- Changes to generic code require regression testing of related systems
