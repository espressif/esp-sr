#ifndef __FLITE_G2P_H__
#define __FLITE_G2P_H__

typedef struct {
    int num_phonemes;
    int phoneme_size;
    char **phonemes;
} flite_g2p_result;

void flite_g2p_result_free(flite_g2p_result *result);

flite_g2p_result *flite_g2p_get_result(const char *grapheme);

void flite_g2p_result_print_string(flite_g2p_result *result, int map_phonemes);

char *flite_g2p_result_get_string(flite_g2p_result *result, int map_phonemes);

char *flite_g2p(const char *graphemes, int map_phonemes);

#endif