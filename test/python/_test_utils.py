# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

import subprocess
import sys
import logging
import os


def is_windows():
    return sys.platform.startswith("win")


def run_subprocess(
    args: list[str],
    cwd: str | bytes | os.PathLike | None = None,
    capture: bool = False,
    dll_path: str | bytes | os.PathLike | None = None,
    shell: bool = False,
    env: dict[str, str] = {},
    log: logging.Logger | None = None,
):
    if log:
        log.info(f"Running subprocess in '{cwd or os.getcwd()}'\n{args}")
    user_env = os.environ.copy()
    user_env.update(env)
    if dll_path:
        if is_windows():
            user_env["PATH"] = dll_path + os.pathsep + user_env["PATH"]
        else:
            if "LD_LIBRARY_PATH" in user_env:
                user_env["LD_LIBRARY_PATH"] += os.pathsep + dll_path
            else:
                user_env["LD_LIBRARY_PATH"] = dll_path

    stdout, stderr = (subprocess.PIPE, subprocess.STDOUT) if capture else (None, None)
    completed_process = subprocess.run(
        args,
        cwd=cwd,
        check=True,
        stdout=stdout,
        stderr=stderr,
        env=user_env,
        shell=shell,
    )

    if log:
        log.debug(
            "Subprocess completed. Return code=" + str(completed_process.returncode)
        )
    return completed_process
