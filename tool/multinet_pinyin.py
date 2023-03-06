from pypinyin import lazy_pinyin
from pypinyin import load_phrases_dict
from pypinyin import load_single_dict
from pypinyin_dict.phrase_pinyin_data import large_pinyin
import argparse


phrases_dict = {
            '开户行': [['ka1i'], ['hu4'], ['hang2']],
            '发卡行': [['fa4'], ['ka3'], ['hang2']],
            '放款行': [['fa4ng'], ['kua3n'], ['hang2']],
            '茧行': [['jia3n'], ['hang2']],
            '行号': [['hang2'], ['ha4o']],
            '各地': [['ge4'], ['di4']],
            '借还款': [['jie4'], ['hua2n'], ['kua3n']],
            '时间为': [['shi2'], ['jia1n'], ['we2i']],
            '为准': [['we2i'], ['zhu3n']],
            '色差': [['se4'], ['cha1']],
            '嗲': [['dia3']],
            '呗': [['bei5']],
            '不': [['bu4']],
            '咗': [['zuo5']],
            '嘞': [['lei5']],
            '掺和': [['chan1'], ['huo5']]
        }

def init_pinyin():
    large_pinyin.load()
    load_phrases_dict(phrases_dict)
    load_single_dict({22320: u'de,di4'})   # 地
    load_single_dict({35843: u'tiao2,diao4'})   #调

def get_pinyin(text):
    label = lazy_pinyin(text)
    label = " ".join(label)

    print("in:", text)
    print("out:", label)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="Chinese Speech Commands")
    parser.add_argument("--text", "-t", type=str,  default=None, help="input text")
    args = parser.parse_args()

    init_pinyin()
    get_pinyin(args.text)