#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from os import walk
from os.path import join as joinpath
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "RFPE01",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)


def find_directories(root_path: str, recursive: bool) -> list[str]:
    found = [root_path]

    for dirpath, dirnames, _ in walk(root_path):
        found += [joinpath(dirpath, x) for x in dirnames]

        if not recursive:
            break

    return found


# Add BTE directories
config.extra_clang_flags.extend(
    [f"-isystem{x}" for x in find_directories("include/revolution/BTE", recursive=True)]
)


# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-enc SJIS",
    "-i include",
    "-i include/MSL",
    "-i include/MSL/internal",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
    "-ir include/revolution/BTE",  # thanks broadcom...(kiwi)
    "-DREVOLUTION",  # BTE changes
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-fp_contract off",
    "-gccinc",
    "-common off",
    "-inline auto",
    "-func_align 4",
]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "Wii/1.0"


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# TODO(texline) Does this actually apply to Wii Fit Plus?
cflags_pedantic = [
    "-w unused",
    "-w missingreturn",
    "-w hidevirtual",
    "-w filecaps",
    "-w sysfilecaps",
    "-w tokenpasting",
    "-w impl_float2int",
    '-pragma "warn_no_explicit_virtual on"',
    "-w err",
]

# NW4R debug library flags
# TODO(texline) verify this
cflags_libnw4r_db = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
]

# NW4R effect library flags
cflags_libnw4r_ef = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
]

# NW4R 3D graphics library flags
cflags_libnw4r_g3d = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
]

# NW4R layout library flags
cflags_libnw4r_lyt = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
]

# NW4R math library flags
cflags_libnw4r_math = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
]

# NW4R sound library flags
cflags_libnw4r_snd = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
]

# NW4R utility library flags
cflags_libnw4r_ut = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
]

# EGG flags
cflags_egg = [
    *cflags_base,
    # *cflags_pedantic,
    "-enc SJIS",
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-i include/nw4r",
    "-ir include/egg",  # TODO(kiwi) remove after refactor
    "-func_align 4",
]

# RVL SDK flags
cflags_rvl = [
    *cflags_base,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
]

# RVLFaceLib flags
cflags_rfl = [
    *cflags_base,
    "-enc SJIS",
    "-fp_contract off",
    "-Cpp_exceptions on",
    "-ipa file",
]

# homeButtonLib flags
cflags_hbm = [
    *cflags_base,
    *cflags_pedantic,
    "-sdata 0",
    "-sdata2 0",
    "-enc SJIS",
    "-lang c++",
    "-RTTI on",
    "-fp_contract off",
    "-ipa file",
    "-i include/nw4r",
    "-i include/homebuttonLib",
]

# Unlike ogws, this repo will only target Wii Fit Plus
cflags_rp = [
    *cflags_base,
    *cflags_pedantic,
    "-enc SJIS",
    "-inline deferred",
    "-fp_contract on",
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-i include/nw4r",
    "-ir include/egg",  # TODO(kiwi) remove after refactor
    "-i include/Pack",
]


Matching = True  # Object matches and should be linked
NonMatching = False  # Object does not match and should not be linked
Equivalent = (
    config.non_matching
)  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "libnw4r_db",
        "mw_version": config.linker_version,
        "cflags": cflags_libnw4r_db,
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/db/db_console.cpp"),
            Object(NonMatching, "nw4r/db/db_exception.cpp"),
        ],
    },
    {
        "lib": "libnw4r_ef",
        "mw_version": config.linker_version,
        "cflags": cflags_libnw4r_ef,
        "progress_category": "nw4r",
        "objects": [
            Object(Matching, "nw4r/ef/ef_draworder.cpp"),
            Object(NonMatching, "nw4r/ef/ef_effect.cpp"),
            Object(NonMatching, "nw4r/ef/ef_effectsystem.cpp"),
            Object(NonMatching, "nw4r/ef/ef_emitter.cpp"),
            Object(NonMatching, "nw4r/ef/ef_particlemanager.cpp"),
            Object(NonMatching, "nw4r/ef/ef_resource.cpp"),
            Object(Matching, "nw4r/ef/ef_handle.cpp"),
            Object(NonMatching, "nw4r/ef/ef_emitterform.cpp"),
            Object(NonMatching, "nw4r/ef/ef_creationqueue.cpp"),
            Object(NonMatching, "nw4r/ef/emform/ef_sphere.cpp"),
            Object(NonMatching, "nw4r/ef/emform/ef_cube.cpp"),
            Object(Matching, "nw4r/ef/ef_drawstrategybuilder.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawstrategyimpl.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawbillboardstrategy.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawdirectionalstrategy.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawfreestrategy.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawlinestrategy.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawpointstrategy.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawstripestrategy.cpp"),
            Object(NonMatching, "nw4r/ef/drawstrategy/ef_drawsmoothstripestrategy.cpp"),
        ],
    },
    {
        "lib": "libnw4r_g3d",
        "mw_version": config.linker_version,
        "cflags": cflags_libnw4r_g3d,
        "progress_category": "nw4r",
        "objects": [
            Object(Matching, "nw4r/g3d/res/g3d_rescommon.cpp"),
            Object(NonMatching, "nw4r/g3d/res/g3d_resmdl.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resshp.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_restev.cpp"),
            Object(NonMatching, "nw4r/g3d/res/g3d_resmat.cpp"),
            Object(Matching, "nw4r/g3d/platform/g3d_gpu.cpp"),
            Object(Matching, "nw4r/g3d/platform/g3d_tmem.cpp"),
            Object(Matching, "nw4r/g3d/platform/g3d_cpu.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_state.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_scnmdl.cpp"),
            Object(Matching, "nw4r/g3d/g3d_fog.cpp"),
            Object(Matching, "nw4r/g3d/g3d_light.cpp"),
            Object(Matching, "nw4r/g3d/g3d_calcvtx.cpp"),
        ],
    },
    {
        "lib": "libnw4r_lyt",
        "mw_version": config.linker_version,
        "cflags": cflags_libnw4r_lyt,
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/lyt/lyt_pane.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_group.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_picture.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_textBox.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_window.cpp"),
            Object(Matching, "nw4r/lyt/lyt_bounding.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_material.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_texMap.cpp"),
            Object(Matching, "nw4r/lyt/lyt_drawInfo.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_animation.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_common.cpp"),
        ],
    },
    {
        "lib": "libnw4r_math",
        "mw_version": config.linker_version,
        "cflags": cflags_libnw4r_math,
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/math/math_arithmetic.cpp"),
            Object(Matching, "nw4r/math/math_triangular.cpp"),
            Object(NonMatching, "nw4r/math/math_geometry.cpp"),
        ],
    },
    {
        "lib": "libnw4r_snd",
        "mw_version": config.linker_version,
        "cflags": cflags_libnw4r_snd,
        "progress_category": "nw4r",
        "objects": [
            Object(Matching, "nw4r/snd/snd_AnimSound.cpp"),
            Object(NonMatching, "nw4r/snd/snd_AxManager.cpp"),
            Object(NonMatching, "nw4r/snd/snd_Bank.cpp"),
            Object(NonMatching, "nw4r/snd/snd_BasicSound.cpp"),
            Object(NonMatching, "nw4r/snd/snd_SoundArchivePlayer.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundHandle.cpp"),
            Object(NonMatching, "nw4r/snd/snd_SoundHeap.cpp"),
        ],
    },
    {
        "lib": "libnw4r_ut",
        "mw_version": config.linker_version,
        "cflags": cflags_libnw4r_ut,
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/ut/ut_list.cpp"),
            Object(Matching, "nw4r/ut/ut_LinkList.cpp"),
            Object(Matching, "nw4r/ut/ut_binaryFileFormat.cpp"),
            Object(Matching, "nw4r/ut/ut_CharStrmReader.cpp"),
            Object(Matching, "nw4r/ut/ut_TagProcessorBase.cpp"),
            Object(NonMatching, "nw4r/ut/ut_IOStream.cpp"),
            Object(Matching, "nw4r/ut/ut_FileStream.cpp"),
            Object(NonMatching, "nw4r/ut/ut_DvdFileStream.cpp"),
            Object(NonMatching, "nw4r/ut/ut_DvdLockedFileStream.cpp"),
        ],
    },
    {
        "lib": "egg",
        "mw_version": config.linker_version,
        "cflags": cflags_egg,
        "progress_category": "egg",
        "objects": [
            Object(NonMatching, "egg/math/eggMatrix.cpp"),
            Object(NonMatching, "egg/core/eggExpHeap.cpp"),
            Object(NonMatching, "egg/core/eggFrmHeap.cpp"),
            Object(Matching, "egg/core/eggHeap.cpp"),
            Object(Matching, "egg/core/eggAllocator.cpp"),
            Object(Matching, "egg/core/eggGraphicsFifo.cpp"),
            Object(Matching, "egg/core/eggSceneManager.cpp"),
            Object(Matching, "egg/core/eggVideo.cpp"),
            Object(NonMatching, "egg/core/eggXfb.cpp"),
            Object(Matching, "egg/core/eggDvdFile.cpp"),
            Object(Matching, "egg/core/eggScene.cpp"),
            Object(NonMatching, "egg/core/eggProcessMeter.cpp"),
            Object(Matching, "egg/core/eggDisposer.cpp"),
            Object(Matching, "egg/core/eggArchive.cpp"),
            Object(NonMatching, "egg/core/eggDecomp.cpp"),
            Object(NonMatching, "egg/core/eggAsyncDisplay.cpp"),
            Object(NonMatching, "egg/core/eggCntFile.cpp"),
            Object(NonMatching, "egg/audio/eggAudioArcPlayerMgr.cpp"),
            Object(NonMatching, "egg/audio/eggAudioExpMgr.cpp"),
            Object(NonMatching, "egg/audio/eggAudioMgr.cpp"),
            Object(Matching, "egg/audio/eggAudioSystem.cpp"),
        ],
    },
    {
        "lib": "runtime",
        "mw_version": config.linker_version,
        "cflags": cflags_runtime,
        "progress_category": "sdk",  # str | List[str]
        "objects": [
            Object(NonMatching, "runtime/global_destructor_chain.c"),
            Object(Matching, "runtime/__init_cpp_exceptions.cpp"),
            Object(Matching, "runtime/Gecko_ExceptionPPC.c"),
            Object(Matching, "runtime/GCN_mem_alloc.c"),
        ],
    },
    {
        "lib": "MSL_C",
        "mw_version": config.linker_version,
        "cflags": cflags_runtime,
        "progress_category": "sdk",
        "objects": [
            Object(NonMatching, "MSL/secure_error.c"),
            Object(NonMatching, "MSL/direct_io.c"),
            Object(NonMatching, "MSL/file_io.c"),
            Object(Matching, "MSL/Math/e_acos.c"),
            Object(Matching, "MSL/Math/e_asin.c"),
            Object(Matching, "MSL/Math/k_rem_pio2.c"),
            Object(Matching, "MSL/Math/k_sin.c"),
            Object(Matching, "MSL/Math/k_tan.c"),
        ],
    },
    {
        "lib": "RVL_SDK",
        "mw_version": config.linker_version,
        "cflags": cflags_rvl,
        "progress_category": "sdk",
        "objects": [
            Object(NonMatching, "revolution/WUD/WUD.c"),
            Object(Matching, "revolution/ARC/arc.c"),
            Object(Matching, "revolution/AX/AXAux.c"),
            Object(NonMatching, "revolution/AX/AXCL.c"),
            Object(Matching, "revolution/AX/AXOut.c"),
            Object(Matching, "revolution/BASE/PPCArch.c"),
            Object(Matching, "revolution/BTE/gki/common/gki_buffer.c"),
            Object(Equivalent, "revolution/BTE/gki/common/gki_time.c"),
            Object(Matching, "revolution/BTE/rvl/gki_ppc.c"),
            Object(Matching, "revolution/DB/db.c"),
            Object(NonMatching, "revolution/DVD/dvdfs.c"),
            Object(NonMatching, "revolution/DVD/dvd.c"),
            Object(Matching, "revolution/DVD/dvdqueue.c"),
            Object(NonMatching, "revolution/DVD/dvderror.c"),
            Object(Matching, "revolution/DVD/dvdidutils.c"),
            Object(NonMatching, "revolution/DVD/dvdfatal.c"),
            Object(NonMatching, "revolution/DVD/dvd_broadway.c"),
            Object(Matching, "revolution/EUART/euart.c"),
            Object(NonMatching, "revolution/EXI/EXIBios.c"),
            Object(NonMatching, "revolution/FS/fs.c"),
            Object(NonMatching, "revolution/GX/GXFifo.c"),
            Object(NonMatching, "revolution/GX/GXInit.c"),
            Object(Matching, "revolution/GX/GXAttr.c"),
            Object(NonMatching, "revolution/GX/GXMisc.c"),
            Object(Matching, "revolution/GX/GXGeometry.c"),
            Object(NonMatching, "revolution/GX/GXFrameBuf.c"),
            Object(Matching, "revolution/GX/GXLight.c"),
            Object(NonMatching, "revolution/GX/GXTexture.c"),
            Object(Matching, "revolution/GX/GXBump.c"),
            Object(NonMatching, "revolution/GX/GXTev.c"),
            Object(NonMatching, "revolution/GX/GXPixel.c"),
            Object(Matching, "revolution/GX/GXDisplayList.c"),
            Object(Matching, "revolution/GX/GXTransform.c"),
            Object(NonMatching, "revolution/GX/GXPerf.c"),
            Object(Matching, "revolution/IPC/ipcMain.c"),
            Object(NonMatching, "revolution/IPC/ipcclt.c"),
            Object(NonMatching, "revolution/KPAD/kpad.c"),
            Object(NonMatching, "revolution/MEM/mem_expHeap.c"),
            Object(Matching, "revolution/MEM/mem_frameHeap.c"),
            Object(Matching, "revolution/MEM/mem_allocator.c"),
            Object(NonMatching, "revolution/MTX/mtx.c"),
            Object(NonMatching, "revolution/NAND/NANDOpenClose.c"),
            Object(NonMatching, "revolution/NAND/NANDCore.c"),
            Object(NonMatching, "revolution/OS/OS.c"),
            Object(NonMatching, "revolution/OS/OSAlarm.c"),
            Object(Matching, "revolution/OS/OSAlloc.c"),
            Object(Matching, "revolution/OS/OSArena.c"),
            Object(Matching, "revolution/OS/OSContext.c"),
            Object(Matching, "revolution/OS/OSError.c"),
            Object(NonMatching, "revolution/OS/OSExec.c"),
            Object(NonMatching, "revolution/OS/OSFatal.c"),
            Object(NonMatching, "revolution/OS/OSFont.c"),
            Object(Matching, "revolution/OS/OSInterrupt.c"),
            Object(NonMatching, "revolution/OS/OSThread.c"),
            Object(Matching, "revolution/OS/OSTime.c"),
            Object(NonMatching, "revolution/SC/scsystem.c"),
            Object(NonMatching, "revolution/SC/scapi.c"),
            Object(NonMatching, "revolution/SI/SIBios.c"),
            Object(NonMatching, "revolution/VI/VI.c"),
            Object(NonMatching, "revolution/VI/i2c.c"),
            Object(NonMatching, "revolution/WPAD/WPAD.c", mw_version="GC/3.0a5.2"),
            Object(
                NonMatching, "revolution/WPAD/WPADHIDParser.c", mw_version="GC/3.0a5.2"
            ),
            Object(NonMatching, "revolution/WBC/wbc.c"),
            Object(Matching, "revolution/OS/__start.c"),
        ],
    },
    {
        "lib": "homebuttonLib",
        "mw_version": "Wii/1.0a",
        "cflags": cflags_hbm,
        "progress_category": "hbm",
        "objects": [
            Object(NonMatching, "homebuttonLib/HBMFrameController.cpp"),
            Object(NonMatching, "homebuttonLib/HBMAnmController.cpp"),
            Object(NonMatching, "homebuttonLib/HBMGUIManager.cpp"),
            Object(NonMatching, "homebuttonLib/HBMController.cpp"),
        ],
    },
    {
        "lib": "RVLFaceLib",
        "mw_version": "GC/3.0a5.2",
        "cflags": cflags_rfl,
        "progress_category": "rfl",
        "objects": [
            Object(NonMatching, "RVLFaceLib/RFL_System.c"),
            Object(Matching, "RVLFaceLib/RFL_NANDLoader.c"),
            Object(NonMatching, "RVLFaceLib/RFL_NANDAccess.c"),
            Object(NonMatching, "RVLFaceLib/RFL_Model.c"),
            Object(NonMatching, "RVLFaceLib/RFL_MakeTex.c"),
            Object(NonMatching, "RVLFaceLib/RFL_Icon.c"),
            Object(NonMatching, "RVLFaceLib/RFL_HiddenDatabase.c"),
            Object(NonMatching, "RVLFaceLib/RFL_Database.c"),
            Object(NonMatching, "RVLFaceLib/RFL_Controller.c"),
            Object(NonMatching, "RVLFaceLib/RFL_MiddleDatabase.c"),
            Object(Matching, "RVLFaceLib/RFL_MakeRandomFace.c"),
            Object(Matching, "RVLFaceLib/RFL_DataUtility.c"),
            Object(Matching, "RVLFaceLib/RFL_Format.c"),
        ],
    },
    {
        "lib": "RP",
        "mw_version": config.linker_version,
        "cflags": cflags_rp,
        "progress_category": "kernel",
        "objects": [
            Object(NonMatching, "Pack/RPKernel/RPSysSystem.cpp"),
            Object(NonMatching, "main.cpp"),
        ],
    },
    {
        "lib": "RP",
        "mw_version": config.linker_version,
        "cflags": cflags_rp,
        "progress_category": "system",
        "objects": [
            Object(NonMatching, "Pack/RPSystem/RPSysRenderMode.cpp"),
            Object(NonMatching, "Pack/RPSystem/RPSysSceneMgr.cpp"),
        ],
    },
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("nw4r", "NW4R"),
    ProgressCategory("egg", "EGG"),
    ProgressCategory("sdk", "RVL SDK"),
    ProgressCategory("hbm", "homeButtonLib"),
    ProgressCategory("rfl", "RVLFaceLib"),
    ProgressCategory("kernel", "RPKernel"),
    ProgressCategory("system", "RPSystem"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
