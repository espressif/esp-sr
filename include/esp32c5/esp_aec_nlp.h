#pragma once

/**
 * @brief Enumeration for AEC (Acoustic Echo Cancellation) NLP (Non-Linear Processing) levels.
 *
 * This enum defines the different levels of NLP aggressiveness used in AEC processing.
 * The levels determine the strength of echo suppression applied during processing.
 */
typedef enum {
    AEC_NLP_LEVEL_NORMAL   = 0,  // Normal level of NLP, suitable for most scenarios.
    AEC_NLP_LEVEL_AGGR     = 1,  // Default: Aggressive level of NLP, provides stronger echo suppression.
    AEC_NLP_LEVEL_VERYAGGR = 2,  // Very aggressive level of NLP, applies the strongest echo suppression.
} aec_nlp_level_t;


