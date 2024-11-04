import pytest
from pytest_embedded import Dut


@pytest.mark.target('esp32s3')
@pytest.mark.env('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        's3',
    ],
)
def test_tts(dut: Dut)-> None:
    # dut.run_all_single_board_cases(group="tts")
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('[tts]')
    dut.expect_unity_test_output(timeout = 1000)

@pytest.mark.target('esp32p4')
@pytest.mark.env('esp32p4')
@pytest.mark.parametrize(
    'config',
    [
        'p4',
    ],
)
def test_tts_p4(dut: Dut)-> None:
    # dut.run_all_single_board_cases(group="tts")
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('[tts]')
    dut.expect_unity_test_output(timeout = 1000)
