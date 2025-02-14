# ======================================= #
#     Nebula Shader Utility (for RHI)     #
#   2025 - @Andromeda08 (Balázs Kovács)   #
# ======================================= #

import io
import os
import glob
import shutil
import subprocess
import sys
from dataclasses import dataclass

# ======================================= #
# --------{ Global Configuration }------- #
# ======================================= #
g_shader_langs = ["glsl", "hlsl"]
g_bin_dir = "bin"
g_target_vulkan_env = "vulkan1.2"
g_excluded_extensions = ["spv", "dxil"]
g_excluded_dirs = []
g_copy_target_dirs = ["../../cmake-build-debug/example"]
g_vulkan_hlsl = False

def fetch_params(arg: str, args: list[str], max_number: int = 1) -> tuple[bool, list[str]]:
    """
    Fetch up to a [max_number] of parameters for a certain command line argument [arg],
    also returns whether [arg] is present or not.
    """
    if max_number < -1:
        raise "Max number of parameters to fetch should be at least 0."
    params = []

    if arg in args:
        if max_number == 0:
            return True, params

        i = args.index(arg) + 1
        while "--" not in args[i] and len(params) < max_number:
            params.append(args[i])
            i += 1
            if i >= len(args):
                break
        return True, params

    return False, params

def check_init() -> tuple[bool, int]:
    num_found_dirs: int = 0
    for lang in g_shader_langs:
        if os.path.exists(lang):
            num_found_dirs += 1
    return num_found_dirs == len(g_shader_langs), num_found_dirs

@dataclass
class Options:
    copy_dirs: list[str]
    copy_output: bool = False
    silent: bool = False
    no_preserve_extension: bool = True
    clean: bool = False
    # Shader compiler flags
    target_env: str = g_target_vulkan_env
    no_debug: bool = False

    def __init__(self, args: list[str]):
        __valid_target_envs = ["1.0", "1.1", "1.2", "1.3"]

        _, target_params = fetch_params("--target", args)
        if len(target_params) > 0:
            env = target_params[0]
            if env not in __valid_target_envs:
                print(f"Invalid target ({env}, valid arguments include {__valid_target_envs}")
            else:
                self.target_env = f"vulkan{env}"

        has_no_debug, _ = fetch_params("--no-debug", args, 0)
        if has_no_debug:
            self.no_debug = True

        has_silent, _ = fetch_params("--silent", args, 0)
        if has_silent:
            self.silent = True

        has_clean, _ = fetch_params("--clean", args, 0)
        if has_clean:
            self.clean = True

        self.copy_dirs = [d for d in g_copy_target_dirs]
        _, copy = fetch_params("--copy", args, 25)
        if len(copy) > 0:
            for directory in copy:
                if directory not in self.copy_dirs:
                    self.copy_dirs.append(directory)

        if len(self.copy_dirs) > 0:
            self.copy_output = True

def cmd_help():
    """Command handler for [--help]"""
    print("# ---{ Nebula Shader Utility }--- #\n"
          "<no args>     \tCompile shaders.\n"
          "--help        \tPrint usage guide.\n"
          "--init        \tGenerate directory structure for shaders in current working directory.\n"
          "--copy <dirs> \tCopy output files to list of space delimited directories.\n"
          "--target <env>\tSpecify the target Vulkan environment. [Default: 1.3]\n"
          "--clean       \tClean output directory before compilation.\n"
          "--no-debug    \tDo not emit debug information.\n"
          "--no-lang-ext \tStrip shader language extension [glsl, hlsl] from output file name."
          "--silent      \tDo not use print.")

def cmd_init():
    """Command handler for [--init]"""
    print("Creating directory structure for NSU.")
    if not os.path.exists(g_bin_dir):
        os.makedirs(g_bin_dir)
    for lang in g_shader_langs:
        if not os.path.exists(lang):
            os.makedirs(lang)

def collect_shaders() -> list[str]:
    result = []
    for lang in g_shader_langs:
        shaders = glob.glob(f"{lang}/**/*", recursive=True)
        shaders = [f for f in shaders if os.path.isfile(f)]
        for shader in shaders:
            # Exclusion checks
            is_excluded = False
            for extension in g_excluded_extensions:
                if f".{extension}" in shader:
                    is_excluded = True
            for directory in g_excluded_dirs:
                if f"{directory}\\" in shader:
                    is_excluded = True
            if not is_excluded:
                result.append(shader)
    return result

def get_hlsl_profile(shader):
    if ".vert" in shader:
        return 'vs_6_1'
    elif ".frag" in shader:
        return 'ps_6_4'
    elif ".comp" in shader:
        return 'cs_6_1'
    elif ".rgen" or ".rchit" or '.rmiss' in shader:
        return 'lib_6_3'
    elif ".mesh" in shader:
        return 'ms_6_6'
    elif ".task" in shader:
        return 'as_6_6'
    else:
        return ''

def get_output_shader_name(shader, options: Options, ext: str = "spv"):
    name = f"{g_bin_dir}\\{shader.rsplit('\\')[-1]}.{ext}"
    if options.no_preserve_extension:
        for lang in g_shader_langs:
            if f".{lang}" in name:
                name = name.replace(f".{lang}", "")
    return name

def compile_hlsl(shader: str, options: Options):
    profile = get_hlsl_profile(shader)
    out_name = get_output_shader_name(shader, options, "spv" if g_vulkan_hlsl else "dxil")

    command = f"dxc -T {profile} -E main {shader} -Fo {out_name}"
    if g_vulkan_hlsl:
        command = f"{command} -spriv -fspv-target-env={options.target_env}"

    if not options.silent:
        with subprocess.Popen(command, stdout=subprocess.PIPE, shell=True) as proc:
            print(f"- {shader.rsplit('\\')[-1]}")
            for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
                print(line.rstrip())

def compile_glsl(shader: str, options: Options):
    command = f"glslangValidator {"" if options.no_debug else "-g"} -o {get_output_shader_name(shader, options)} -V {shader} --target-env {options.target_env}"
    with subprocess.Popen(command, stdout=subprocess.PIPE, shell=True) as proc:
        if not options.silent:
            for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
                print(f"- {line.rstrip()}")

def cmd_compile(options: Options):
    shaders = collect_shaders()

    if not os.path.exists(g_bin_dir):
        if not options.silent:
            print(f"[Info] Created output directory: [./{g_bin_dir}]")
        os.makedirs(g_bin_dir)
    else:
        if options.clean:
            old_files = glob.glob(f"{g_bin_dir}/*")
            for f in old_files:
                os.remove(f)

    if not options.silent:
        print(f"[Info] Compiling {len(shaders)} shader(s). [target_env={options.target_env}] [debug={not options.no_debug}]")

    for shader in shaders:
        if "hlsl" in shader:
            compile_hlsl(shader, options)
        else:
            compile_glsl(shader, options)

    return

def cmd_copy(options: Options):
    if not options.silent:
        print(f"[Info] Copying shader(s) to: {options.copy_dirs}")
    compiled_shaders = glob.glob(f"bin/*")
    for shader in compiled_shaders:
        for target_dir in options.copy_dirs:
            if not os.path.exists(target_dir):
                os.mkdir(target_dir)
            shutil.copy2(shader, target_dir)

def main():
    args = sys.argv

    if "--help" in args:
        cmd_help()
        return

    if "--init" in args:
        cmd_init()
        return

    is_complete, num_langs = check_init()
    if not is_complete:
        print("[Info] Run with the flag --init to create initial directory setup in the current working directory.")
        return
    if num_langs < len(g_shader_langs):
        print("[Warn] Not all specified shader language directories are present.")

    options = Options(args)

    cmd_compile(options)

    if options.copy_output:
        cmd_copy(options)

main()
