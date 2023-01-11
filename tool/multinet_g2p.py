from g2p_en import G2p
import argparse
import numpy as np
import pandas as pd
import sentencepiece as spm

def english_g2p(text, alphabet=None):
    g2p = G2p()
    out = ""

    if alphabet is None:
        alphabet={"AE1": "a", "N": "N", " ": " ", "OW1": "b", "V": "V", "AH0": "c", "L": "L", "F": "F", "EY1": "d", "S": "S", "B": "B", "R": "R", "AO1": "e", "D": "D", "AH1": "c", "EH1": "f", "OW0": "b", "IH0": "g", "G": "G", "HH": "h", "K": "K", "IH1": "g", "W": "W", "AY1": "i", "T": "T", "M": "M", "Z": "Z", "DH": "j", "ER0": "k", "P": "P", "NG": "l", "IY1": "m", "AA1": "n", "Y": "Y", "UW1": "o", "IY0": "m", "EH2": "f", "CH": "p", "AE0": "a", "JH": "q", "ZH": "r", "AA2": "n", "SH": "s", "AW1": "t", "OY1": "u", "AW2": "t", "IH2": "g", "AE2": "a", "EY2": "d", "ER1": "k", "TH": "v", "UH1": "w", "UW2": "o", "OW2": "b", "AY2": "i", "UW0": "o", "AH2": "c", "EH0": "f", "AW0": "t", "AO2": "e", "AO0": "e", "UH0": "w", "UH2": "w", "AA0": "n", "AY0": "i", "IY2": "m", "EY0": "d", "ER2": "k", "OY2": "u", "OY0": "u"}

    text_list = text.split(";")
    for item in text_list:
        item = item.split(",")
        for phrase in item:
            labels = g2p(phrase)
            for char in labels:
                if item not in alphabet:
                    print("skip %s, not found in alphabet")
                    continue
                else:
                    out += alphabet[char]
            if phrase != item[-1]:
                out += ','
        out += ";"

    print("in:", text)
    print("out:", out)
    
    return out


def spm_encode_text(text, sp, enable_sampling=False):
    text = text.upper()
    def text_norm(str_list):
        out_str = ''
        for i in range(len(out)):
            out_str += out[i].replace("▁", "_")
            out_str += ' '
        return out_str[:-1]
    out_str = ''
    if enable_sampling:
        for i in range(5):
            out = sp.encode(text, out_type=str, nbest_size=-1, enable_sampling=True, alpha=0.01)
            out_str += text_norm(out) + ","
    else:
        out = sp.encode(text, out_type=str, enable_sampling=enable_sampling)
        out_str += text_norm(out)

    return out_str


def encode_text(text, bpe_model_file, enable_sampling=False):
    sp = spm.SentencePieceProcessor()
    sp.load(bpe_model_file)
    text = text
    out_str = spm_encode_text(text, sp, enable_sampling)
    print("in:", text)
    print("out:", out_str)

    return out_str

def encode_csv(csv_file, bpe_model_file, enable_sampling=False):
    text_list = pd.read_csv(csv_file).values
    sp = spm.SentencePieceProcessor()
    sp.load(bpe_model_file)
    out_str = ""

    for text in text_list:
        in_text = text[1]
        out_str += spm_encode_text(in_text, sp, enable_sampling)
        out_str += "\n"

    print(out_str)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(prog="English Speech Commands G2P")

    parser.add_argument("--text", "-t", type=str,  default=None, help="input text")
    parser.add_argument("--bpe_model", "-m", type=str,  default=None, help="bpe model path")
    parser.add_argument("--enable_sampling", "-s", action="store_true", help="enable encode sampling, output more segmentation")
    parser.add_argument("--csv_file", "-c", type=str,  default=None, help="text csv file")
    args = parser.parse_args()
    
    if args.bpe_model is not None:
        if args.text is not None:
            encode_text(args.text, args.bpe_model, args.enable_sampling)
        else:
            encode_csv(args.csv_file, args.bpe_model, args.enable_sampling)
    else:
        english_g2p(args.text, args.alphabet_map)
