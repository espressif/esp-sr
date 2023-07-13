import pytest
from pytest_embedded import Dut

@pytest.mark.target('esp32s3')
@pytest.mark.env('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'xiaoxin',
    ],
)
def test_tts(dut: Dut)-> None:
    dut.run_all_single_board_cases(group="tts")
