from g2p_en import G2p
import argparse
import numpy as np
import pandas

def english_g2p(text, alphabet=None):
    g2p = G2p()
    out = ""

    if alphabet is None:
        alphabet={"AE1": "a", "N": "N", " ": " ", "OW1": "b", "V": "V", "AH0": "c", "L": "L", "F": "F", "EY1": "d", "S": "S", "B": "B", "R": "R", "AO1": "e", "D": "D", "AH1": "c", "EH1": "f", "OW0": "b", "IH0": "g", "G": "G", "HH": "h", "K": "K", "IH1": "g", "W": "W", "AY1": "i", "T": "T", "M": "M", "Z": "Z", "DH": "j", "ER0": "k", "P": "P", "NG": "l", "IY1": "m", "AA1": "n", "Y": "Y", "UW1": "o", "IY0": "m", "EH2": "f", "CH": "p", "AE0": "a", "JH": "q", "ZH": "r", "AA2": "n", "SH": "s", "AW1": "t", "OY1": "u", "AW2": "t", "IH2": "g", "AE2": "a", "EY2": "d", "ER1": "k", "TH": "v", "UH1": "w", "UW2": "o", "OW2": "b", "AY2": "i", "UW0": "o", "AH2": "c", "EH0": "f", "AW0": "t", "AO2": "e", "AO0": "e", "UH0": "w", "UH2": "w", "AA0": "n", "AY0": "i", "IY2": "m", "EY0": "d", "ER2": "k", "OY2": "u", "OY0": "u"}

    text_list = text.split(";")
    count = 1
    for item in text_list:
        item_out = ""  # Danh sách tạm thời cho kết quả của mỗi item
        item = item.split(",")
        for phrase in item:
            labels = g2p(phrase)
            word_out = ""
            for char in labels:
                if char not in alphabet:
                    print("skip %s, not found in alphabet" % char)
                    continue
                else:
                    word_out += alphabet[char]
            item_out += word_out
            if phrase != item[-1]:
                item_out += ','
        out += f"{count},{phrase},{item_out}\n"
        count += 1

    print(out)

if __name__ == "__main__":

    parser = argparse.ArgumentParser(prog="English Speech Commands G2P")

    parser.add_argument("--text", "-t", type=str,  default=None, help="input text")
    parser.add_argument("--alphabet_map", "-a", type=str,  default=None, help="the json file to map label into classes of model")
    parser.add_argument('-c', '--c_file', help="name of .c files")
    parser.add_argument('-head', '--h_file', help="name of .h files")
    args = parser.parse_args()
    
    if args.text is not None:
        english_g2p(args.text, args.alphabet_map)

