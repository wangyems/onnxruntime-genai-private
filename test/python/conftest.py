# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License

import functools
import os
import sys

import pytest
from _test_utils import run_subprocess


def pytest_addoption(parser):
    parser.addoption(
        "--test_models",
        help="Path to the current working directory",
        type=str,
        required=True,
    )


def get_path_for_model_and_device(data_path, model_name, device):
    return os.path.join(data_path, device, model_name)


@pytest.fixture
def phi2_for(request):
    return functools.partial(
        get_path_for_model_and_device,
        request.config.getoption("--test_models"),
        "phi-2",
    )


@pytest.fixture
def gemma_for(request):
    return functools.partial(
        get_path_for_model_and_device,
        request.config.getoption("--test_models"),
        "gemma",
    )


@pytest.fixture
def llama_for(request):
    return functools.partial(
        get_path_for_model_and_device,
        request.config.getoption("--test_models"),
        "llama",
    )


@pytest.fixture
def path_for_model(request):
    return functools.partial(
        get_path_for_model_and_device, request.config.getoption("--test_models")
    )


@pytest.fixture
def test_data_path(request):
    return request.config.getoption("--test_models")
