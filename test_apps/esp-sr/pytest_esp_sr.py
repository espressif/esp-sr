import pytest
from pytest_embedded import Dut

@pytest.mark.target('esp32s3')
@pytest.mark.env('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'mn5q8_cn',
        'mn5q8_en',
        'mn6_cn',
        'mn6_en',
        'mn7_en',
    ],
)
def test_multinet(dut: Dut)-> None:
    # dut.run_all_single_board_cases(group="mn")
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('[mn]')
    dut.expect_unity_test_output(timeout = 1000)


@pytest.mark.target('esp32s3')
@pytest.mark.env('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'mn5q8_en',
        'wn9_hilexin',
    ],
)
def test_wakenet(dut: Dut)-> None:
    # dut.run_all_single_board_cases(group="wn")
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('[wn]')
    dut.expect_unity_test_output(timeout = 1000)


@pytest.mark.target('esp32s3')
@pytest.mark.env('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'wn9_hilexin',
    ],
)
def test_sr_afe(dut: Dut)-> None:
    # dut.run_all_single_board_cases(group="afe")
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('[afe_sr]')
    dut.expect_unity_test_output(timeout = 1000)


@pytest.mark.target('esp32s3')
@pytest.mark.env('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'nsnet1',
    ],
)
def test_vc_afe(dut: Dut)-> None:
    # dut.run_all_single_board_cases(group="afe")
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('[afe_vc]')
    dut.expect_unity_test_output(timeout = 100000)