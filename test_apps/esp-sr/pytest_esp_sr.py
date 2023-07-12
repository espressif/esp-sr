import pytest
from pytest_embedded import Dut

# @pytest.mark.target('esp32s3')
# @pytest.mark.parametrize(
#     'config',
#     [
#         'mn5q8_cn',
#         'mn6_cn',
#     ],
# )
# def test_multinet(dut: Dut)-> None:
#     dut.run_all_single_board_cases(group="mn")


@pytest.mark.target('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'mn5q8_cn',
        'wn9_hilexin',
    ],
)
def test_wakenet(dut: Dut)-> None:
    dut.run_all_single_board_cases(group="wn")


@pytest.mark.target('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'wn9_hilexin',
    ],
)
def test_afe(dut: Dut)-> None:
    dut.run_all_single_board_cases(group="afe")