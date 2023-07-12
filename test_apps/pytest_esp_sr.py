'''
Steps to run these cases:
- Build
  - . ${IDF_PATH}/export.sh
  - pip install idf_build_apps
  - python test_apps/build_apps.py test_apps -t esp32s3
- Test
  - pip install -r tools/requirements/requirement.pytest.txt
  - pytest test_apps --target esp32s2
'''
import pytest
from pytest_embedded import Dut

@pytest.mark.target('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'mn5q8_cn',
        'mn6_cn',
    ],
)
def test_multinet(dut: Dut)-> None:
    dut.run_all_single_board_cases(group="mn")


@pytest.mark.target('esp32s3')
@pytest.mark.parametrize(
    'config',
    [
        'hilexin',
        'hiesp',
    ],
)
def test_multinet(dut: Dut)-> None:
    dut.run_all_single_board_cases(group="mn")