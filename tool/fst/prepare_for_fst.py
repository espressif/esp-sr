import argparse
from pathlib import Path
from typing import List, Set, Tuple

import sentencepiece as spm


def process_commands(infile: List[str], sp: spm.SentencePieceProcessor
                     ) -> Tuple[List[str], Set[int]]:
    out_commands = []
    tokens = set()

    for line in infile:
        command_id = line.split()[0]
        command = ' '.join(line.split()[1:])
        command_tokens = sp.encode(command, out_type=str)
        for token in command_tokens:
            tokens.add(token)
        command_tokens = [command_id] + command_tokens
        out_commands.append('\t'.join(command_tokens))
    return out_commands, tokens


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--infile', type=str, required=True,
                        help='the text file of commands id and commands.')
    parser.add_argument('--bpe-model', type=str, default='bpe.model',
                        help='subword bpe model file.')
    parser.add_argument('--out-command-list', type=str,
                        default='commands_tokens.txt',
                        help='the output subword commands text filename.')
    parser.add_argument('--out-token-symbols', type=str,
                        default='tokens.txt',
                        help='the output token to subword id mapping.')
    args = parser.parse_args()

    if not Path(args.infile).is_file():
        raise FileNotFoundError(args.infile)

    if not Path(args.bpe_model).is_file():
        raise FileNotFoundError(args.bpe_model)

    with open(args.infile) as f:
        infile = f.readlines()
    infile = [x.strip() for x in infile]

    sp = spm.SentencePieceProcessor()
    sp.load(args.bpe_model)

    out_commands, tokens = process_commands(infile, sp)

    token_symbols = []
    for i in range(sp.vocab_size()):
        if sp.id_to_piece(i) in tokens or i == 0:
            token_symbols.append(f'{sp.id_to_piece(i)}\t{i}')

    with open(args.out_command_list, 'wt') as f:
        f.write('\n'.join(out_commands))
        f.write('\n')

    with open(args.out_token_symbols, 'wt') as f:
        f.write('\n'.join(token_symbols))
        f.write('\n')
