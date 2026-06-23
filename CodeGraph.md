# CodeGraph Reference Manual — Ikemen GO + SSZ

CodeGraph is a tree-sitter-parsed AST knowledge graph of every symbol, edge, and
file in the project. Reads are sub-millisecond and return structural information
grep cannot provide.

**Index stats:** 50 files, 1,888 nodes, 6,403 edges, 4.68 MB DB.

**Availability:** CodeGraph runs as an MCP server (`codegraph serve --mcp`
configured in `opencode.jsonc`). Invoke tools directly by name — they are
listed in the agent's available tool set.

---

## 1. Tools Overview

| Tool | Purpose | Returns |
|------|---------|---------|
| `codegraph_status` | Index health / stats | Node counts, DB size |
| `codegraph_files` | File listing by path / glob | Filtered tree or flat list |
| `codegraph_search` | Find symbol by name | Name, kind, file, line |
| `codegraph_node` | Get symbol detail | Signature, location, source code |
| `codegraph_callers` | Who calls this symbol | Caller list |
| `codegraph_callees` | What this symbol calls | Callee list |
| `codegraph_impact` | What would break if I change X | N-depth dependency fan-out |
| `codegraph_explore` | Deep multi-file survey | Full source from relevant files |

CLI equivalents (`codegraph status`, `codegraph query`, etc.) also work in
`bash` but the MCP tools are preferred — they return structured, parsed data.

---

## 2. Tool-by-Tool: Real Examples

### 2.1 `codegraph_status` — Index health

```
> codegraph_codegraph_status
```

**Result:** Files indexed, total nodes, edges, DB size, breakdown by node kind
(function, struct, enum, variable, etc.).

**Use when:** You want to confirm the index is healthy before a complex query,
or you suspect a large file / third-party tree was indexed unexpectedly.

---

### 2.2 `codegraph_files` — File listing

```
> codegraph_codegraph_files
```

**Result:** Full tree of all 50 indexed files under `main/` — SDL plugin,
SSZ compiler core (jitcompiler.hpp, sourcetree.hpp, x86.hpp), 14 plugin
subsystems, static registration headers.

```
> codegraph_codegraph_files path=main/ssz format=flat
```

**Result:** All SSZ core files — `jitcompiler.hpp`, `sourcetree.hpp`,
`x86.hpp`, `ssz.cpp`, `sszdef.h`, `typeid.h`, `tokenkind.h`, etc.

**Use when:** You need to survey the directory structure, find files by pattern,
or understand the project layout before diving into a specific area.

---

### 2.3 `codegraph_search` — Find symbol by name

```
> codegraph_search query=PlayBGM
```

**Result:**

| Symbol | Kind | File | Line |
|--------|------|------|------|
| `PlayBGM` | function | `main/sdlplugin/sdlplugin.cpp` | 2465 |

```
> codegraph_search query=TUserFunc kind=function
```

**Result:** All `TUserFunc` instantiations — the SSZ plugin function
registrations across all 14 subsystems (PlayBGM, PlayVideo, MemMarkBefore,
OggVorbisOpen, etc.).

**Use when:** You know the symbol name but not its location.

---

### 2.4 `codegraph_node` — Symbol detail + source

```
> codegraph_node symbol=MemRecord includeCode=true
```

**Result:** Full function signature, location, and source code from
`main/mem_profiler.hpp:42` — the snapshot recorder.

```
> codegraph_node symbol=PlayVLCVideo
```

**Result:** Location only — `main/sdlplugin/sdlplugin.cpp:1668`. Use
`includeCode=false` (default) when you just need the file:line to navigate.

**Use when:** You need the exact signature, source, or docstring of a known
symbol. Prefer this over opening the file with Read — it's faster and gives
you parsed structure.

---

### 2.5 `codegraph_callers` — Who calls this?

```
> codegraph_callers symbol=GetLiveMemory
```

**Result:** 1 caller — `MemPrintRanking` in `main/mem_profiler.hpp:55`.

```
> codegraph_callers symbol=PlayVLCVideo
```

**Result:** 1 caller — `PlayVideo` in `main/sdlplugin/sdlplugin.cpp:1838`.

**Use when:** You want to understand who depends on a function, or trace the
call chain backward from an effect.

---

### 2.6 `codegraph_callees` — What does it call?

```
> codegraph_callees symbol=MemPrintRanking
```

**Result:** 1 callee — `GetLiveMemory` in `main/mem_profiler.hpp:21`.

**Use when:** You need to understand what a function depends on, trace its
implementation flow, or estimate the blast radius of adding a new dependency.

---

### 2.7 `codegraph_impact` — Blast radius analysis

```
> codegraph_impact symbol=GetLiveMemory depth=2
```

**Result:** Changing `GetLiveMemory` affects **4 symbols** (MemRecord,
MemPrintRanking, SafePrintRanking, and the mem_profiler.hpp file node).

```
> codegraph_impact symbol=PlayBGM depth=2
```

**Result:** Everything that `PlayBGM` touches, plus everything that touches
those — good for assessing risky refactors.

**Use when:** You're planning to change a widely-used utility (mutex, log
function, config API) and need to know how many places to audit.

---

### 2.8 `codegraph_explore` — Deep survey

```
> codegraph_explore query="MemRecord mem_profiler"
```

**Result:** Full source from `main/mem_profiler.hpp`, `main/ssz/ssz.cpp`,
and related files — includes the mem snapshot struct, recording function,
ranking print, macros, and the SSZ allocator hooks.

```
> codegraph_explore query="PlayBGM PlayVideo PlayVLCVideo"
```

**Result:** Full source from `main/sdlplugin/sdlplugin.cpp` — all video and
audio playback functions with the VLC and SDL_mixer integration.

```
> codegraph_explore query="static_plugin_registry PluginUtil"
```

**Result:** Full source from `main/ssz/static_plugin_registry.hpp` and
`main/ssz/pluginutil.hpp` — the plugin registration infrastructure.

**Use when:** You need to deeply understand an unfamiliar subsystem. This is
token-heavy but replaces many individual Read/node calls.

---

## 3. When to Use CodeGraph vs. Grep

| Question type | Tool | Why |
|--------------|------|-----|
| "Where is X defined?" | `codegraph_search` | Sub-millisecond AST lookup |
| "What symbol is at line N?" | `codegraph_node` | Parsed structure |
| "What calls function Y?" | `codegraph_callers` | Edge traversal, not regex |
| "What does Y call?" | `codegraph_callees` | Edge traversal |
| "What would break if I changed Z?" | `codegraph_impact` | N-depth dependency fan-out |
| "Survey a new module" | `codegraph_explore` | One-call full source |
| "Find literal string 'Connection refused'" | `grep` | CodeGraph indexes symbols, not strings |
| "Find a comment mentioning TODO" | `grep` | Comments are not AST nodes |
| "Find every call to `MemMarkBefore` in Lua" | `grep` (`.lua` include) | Lua code is not C++, won't be in the AST index |

**Rule of thumb:** Structural → CodeGraph. Literal text → grep. File detail → Read.

---

## 4. Index Lag

CodeGraph re-indexes files within ~500ms of save. If you edit a file and
immediately query it, the old index may be returned. Wait 1 second or re-query
if you get stale results.

---

## 5. Project-Specific Patterns

### Finding SSZ plugin function registrations

```
> codegraph_search query=TUserFunc kind=function
```

### Tracing the JIT compiler entry point

```
> codegraph_node symbol=Compile includeCode=true
> codegraph_callers symbol=Compile
```

### Understanding the memory profiler

```
> codegraph_explore query="MemRecord mem_profiler GetLiveMemory"
```

### Finding all SSZ source tree operations

```
> codegraph_search query=SourceTree kind=class
> codegraph_nodes symbol=SourceTree includeCode=true
```

### Checking the x86 codegen backend

```
> codegraph_files path=main/ssz pattern=x86* format=flat
> codegraph_node symbol=CodeGenerator includeCode=true
```

---

## 6. Quick Reference

| Goal | Command |
|------|---------|
| Is the index ready? | `codegraph_status` |
| List all source files | `codegraph_files` |
| Find a function | `codegraph_search query=funcname` |
| Get function source | `codegraph_node symbol=funcname includeCode=true` |
| Who calls me? | `codegraph_callers symbol=funcname` |
| What do I call? | `codegraph_callees symbol=funcname` |
| Blast radius | `codegraph_impact symbol=funcname depth=2` |
| Deep survey | `codegraph_explore query="symbol1 symbol2 symbol3"` |
