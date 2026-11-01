# Parâmetros 0.3.0

70 parâmetros; os primeiros 64 mantêm seus IDs e ordem da versão 0.2.1. As faixas históricas são aproximações de software. Consulte [TAPE-DSP.md](TAPE-DSP.md) para condições de ativação, rotas e limites.

| ID | Controle | Padrão | Tipo |
|---|---|---|---|
| `machine_version` | Machine type | 15/7.5 | choice |
| `speed_select` | SPEED | Fast | choice |
| `eq_standard` | EQ | NAB | choice |
| `vu_jumper` | VU line reference | 4 | choice |
| `tape_type` | Tape | SCOTCH 3M 250 | choice |
| `bias_fast` | BIAS SETTING FAST | 0 | float |
| `erase_fast` | ERASE FAST | 0 | float |
| `bias_slow` | BIAS SETTING SLOW | 0 | float |
| `erase_slow` | ERASE SLOW | 0 | float |
| `osc_stop` | Oscillator stop | False | bool |
| `varispeed_on` | VARISPEED | False | bool |
| `varispeed_semitones` | Varispeed | 0 | float |
| `calibrate` | CALIBRATE | False | bool |
| `mastersafe` | MASTERSAFE | False | bool |
| `sync_range` | Sync range | wide | choice |
| `in_sens_plus7` | Input sensitivity | normal | choice |
| `treble_boost_tc` | Lower speed treble boost time | 0 | choice |
| `treble_boost_amt` | Lower speed treble boost amount | 1 | choice |
| `repro_input` | Repro input | normal | choice |
| `stereo_link` | Stereo link | True | bool |
| `transport_modelling` | Transport modelling | False | bool |
| `alignment_dbfs` | 0 VU alignment | -18 | float |
| `ch1_src` | Monitor | REPRO | choice |
| `ch1_rec_input_level` | Record Input Level | 0 | float |
| `ch1_rep_out1_level` | Output 1 Level | 0 | float |
| `ch1_syn_out2_level` | Output 2 Level | 0 | float |
| `ch1_rec_fast_level` | REC FAST level | 0 | float |
| `ch1_rec_fast_treble` | REC FAST treble | 0 | float |
| `ch1_rec_slow_level` | REC SLOW level | 0 | float |
| `ch1_rec_slow_treble` | REC SLOW treble | 0 | float |
| `ch1_rep_fast_level` | REP FAST level | 0 | float |
| `ch1_rep_fast_treble` | REP FAST treble | 0 | float |
| `ch1_rep_fast_bass` | REP FAST bass | 0 | float |
| `ch1_rep_slow_level` | REP SLOW level | 0 | float |
| `ch1_rep_slow_treble` | REP SLOW treble | 0 | float |
| `ch1_rep_slow_bass` | REP SLOW bass | 0 | float |
| `ch1_syn_fast_level` | SYN FAST level | 0 | float |
| `ch1_syn_fast_treble` | SYN FAST treble | 0 | float |
| `ch1_syn_fast_bass` | SYN FAST bass | 0 | float |
| `ch1_syn_slow_level` | SYN SLOW level | 0 | float |
| `ch1_syn_slow_treble` | SYN SLOW treble | 0 | float |
| `ch1_syn_slow_bass` | SYN SLOW bass | 0 | float |
| `ch1_ch_adjust` | CH. ADJUST | 0 | float |
| `ch2_src` | Monitor | REPRO | choice |
| `ch2_rec_input_level` | Record Input Level | 0 | float |
| `ch2_rep_out1_level` | Output 1 Level | 0 | float |
| `ch2_syn_out2_level` | Output 2 Level | 0 | float |
| `ch2_rec_fast_level` | REC FAST level | 0 | float |
| `ch2_rec_fast_treble` | REC FAST treble | 0 | float |
| `ch2_rec_slow_level` | REC SLOW level | 0 | float |
| `ch2_rec_slow_treble` | REC SLOW treble | 0 | float |
| `ch2_rep_fast_level` | REP FAST level | 0 | float |
| `ch2_rep_fast_treble` | REP FAST treble | 0 | float |
| `ch2_rep_fast_bass` | REP FAST bass | 0 | float |
| `ch2_rep_slow_level` | REP SLOW level | 0 | float |
| `ch2_rep_slow_treble` | REP SLOW treble | 0 | float |
| `ch2_rep_slow_bass` | REP SLOW bass | 0 | float |
| `ch2_syn_fast_level` | SYN FAST level | 0 | float |
| `ch2_syn_fast_treble` | SYN FAST treble | 0 | float |
| `ch2_syn_fast_bass` | SYN FAST bass | 0 | float |
| `ch2_syn_slow_level` | SYN SLOW level | 0 | float |
| `ch2_syn_slow_treble` | SYN SLOW treble | 0 | float |
| `ch2_syn_slow_bass` | SYN SLOW bass | 0 | float |
| `ch2_ch_adjust` | CH. ADJUST | 0 | float |
| `tape_drive` | Tape drive | 0 | float |
| `tape_mix` | Tape wet mix | 100 | float |
| `tape_noise` | Tape hiss | -80 | float |
| `tape_wow` | Wow depth | 15 | float |
| `tape_flutter` | Flutter depth | 10 | float |
| `tape_record` | Tape processing enabled (REC) | True | bool |
