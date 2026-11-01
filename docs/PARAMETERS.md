# Mapeamento de parâmetros v1

Fonte: texto anexado pelo usuário. Os manuais SI 66/81 E e A800 Audio Adjustments ainda não foram conferidos. Este contrato descreve o alvo completo; a implementação parcial e os controles pendentes constam em IMPLEMENTATION.md. Não comprova fidelidade sonora.

IDs são estáveis; version_hint = 1 deve ser preservado no futuro APVTS. Choices devem manter ordem e índice. Defaults são valores semânticos, a serem convertidos pelo adaptador JUCE.

| ID | Controle | Tipo / opções | Padrão | Cartão | Efeito |
|---|---|---|---|---|---|
| machine_version | Machine type | 7.5/3.75 / 15/7.5 / 30/15 | 15/7.5 | — | audio |
| speed_select | SPEED | Slow / Fast | Fast | — | audio |
| eq_standard | EQ | NAB / CCIR | NAB | — | audio |
| vu_jumper | VU line reference | 0 / 4 / 6 / 8 | 4 | — | meter |
| tape_type | Tape | SCOTCH 3M 226 / SCOTCH 3M 206 / SCOTCH 3M 263 / SCOTCH 3M 250 / SCOTCH 3M 256 / AGFA PEM 468 / AGFA PER 525 / AMPEX 406 / AMPEX 456 / BASF LGR 30 P / BASF SPR 50 LH / EMI 816/817 / PYRAL CJ 90 / AGFA PER 528 / BASF LGR 50 | SCOTCH 3M 250 | — | audio |
| bias_fast | BIAS SETTING FAST | float | 0 | 1.180.714 | audio |
| erase_fast | ERASE FAST | float | 0 | 1.180.714 | visual |
| bias_slow | BIAS SETTING SLOW | float | 0 | 1.180.714 | audio |
| erase_slow | ERASE SLOW | float | 0 | 1.180.714 | visual |
| osc_stop | Oscillator stop | bool | False | — | visual |
| varispeed_on | VARISPEED | bool | False | — | audio |
| varispeed_semitones | Varispeed | float | 0 | — | audio |
| calibrate | CALIBRATE | bool | False | — | meter_and_calibration |
| mastersafe | MASTERSAFE | bool | False | — | audio |
| sync_range | Sync range | wide / narrow | wide | 1.180.711 | audio |
| in_sens_plus7 | Input sensitivity | normal / +7 dB | normal | 1.180.712 | audio |
| treble_boost_tc | Lower speed treble boost time | 0 / C / B / A | 0 | 1.180.712 | audio |
| treble_boost_amt | Lower speed treble boost amount | 1 / 2 / 3 / 4 | 1 | 1.180.712 | audio |
| repro_input | Repro input | normal / external | normal | 1.180.710 | visual |
| stereo_link | Stereo link | bool | True | — | editor_link |
| transport_modelling | Transport modelling | bool | False | — | audio |
| alignment_dbfs | 0 VU alignment | float | -18 | — | audio |
| ch1_src | Monitor | INPUT / REPRO / SYNC | REPRO | — | audio |
| ch1_rec_input_level | Record Input Level | float | 0 | 1.180.712 | audio |
| ch1_rep_out1_level | Output 1 Level | float | 0 | 1.180.710 | audio |
| ch1_syn_out2_level | Output 2 Level | float | 0 | 1.180.711 | audio |
| ch1_rec_fast_level | REC FAST level | float | 0 | 1.180.712 | audio |
| ch1_rec_fast_treble | REC FAST treble | float | 0 | 1.180.712 | audio |
| ch1_rec_slow_level | REC SLOW level | float | 0 | 1.180.712 | audio |
| ch1_rec_slow_treble | REC SLOW treble | float | 0 | 1.180.712 | audio |
| ch1_rep_fast_level | REP FAST level | float | 0 | 1.180.710 | audio |
| ch1_rep_fast_treble | REP FAST treble | float | 0 | 1.180.710 | audio |
| ch1_rep_fast_bass | REP FAST bass | float | 0 | 1.180.710 | audio |
| ch1_rep_slow_level | REP SLOW level | float | 0 | 1.180.710 | audio |
| ch1_rep_slow_treble | REP SLOW treble | float | 0 | 1.180.710 | audio |
| ch1_rep_slow_bass | REP SLOW bass | float | 0 | 1.180.710 | audio |
| ch1_syn_fast_level | SYN FAST level | float | 0 | 1.180.711 | audio |
| ch1_syn_fast_treble | SYN FAST treble | float | 0 | 1.180.711 | audio |
| ch1_syn_fast_bass | SYN FAST bass | float | 0 | 1.180.711 | audio |
| ch1_syn_slow_level | SYN SLOW level | float | 0 | 1.180.711 | audio |
| ch1_syn_slow_treble | SYN SLOW treble | float | 0 | 1.180.711 | audio |
| ch1_syn_slow_bass | SYN SLOW bass | float | 0 | 1.180.711 | audio |
| ch1_ch_adjust | CH. ADJUST | float | 0 | 1.180.714 | audio |
| ch2_src | Monitor | INPUT / REPRO / SYNC | REPRO | — | audio |
| ch2_rec_input_level | Record Input Level | float | 0 | 1.180.712 | audio |
| ch2_rep_out1_level | Output 1 Level | float | 0 | 1.180.710 | audio |
| ch2_syn_out2_level | Output 2 Level | float | 0 | 1.180.711 | audio |
| ch2_rec_fast_level | REC FAST level | float | 0 | 1.180.712 | audio |
| ch2_rec_fast_treble | REC FAST treble | float | 0 | 1.180.712 | audio |
| ch2_rec_slow_level | REC SLOW level | float | 0 | 1.180.712 | audio |
| ch2_rec_slow_treble | REC SLOW treble | float | 0 | 1.180.712 | audio |
| ch2_rep_fast_level | REP FAST level | float | 0 | 1.180.710 | audio |
| ch2_rep_fast_treble | REP FAST treble | float | 0 | 1.180.710 | audio |
| ch2_rep_fast_bass | REP FAST bass | float | 0 | 1.180.710 | audio |
| ch2_rep_slow_level | REP SLOW level | float | 0 | 1.180.710 | audio |
| ch2_rep_slow_treble | REP SLOW treble | float | 0 | 1.180.710 | audio |
| ch2_rep_slow_bass | REP SLOW bass | float | 0 | 1.180.710 | audio |
| ch2_syn_fast_level | SYN FAST level | float | 0 | 1.180.711 | audio |
| ch2_syn_fast_treble | SYN FAST treble | float | 0 | 1.180.711 | audio |
| ch2_syn_fast_bass | SYN FAST bass | float | 0 | 1.180.711 | audio |
| ch2_syn_slow_level | SYN SLOW level | float | 0 | 1.180.711 | audio |
| ch2_syn_slow_treble | SYN SLOW treble | float | 0 | 1.180.711 | audio |
| ch2_syn_slow_bass | SYN SLOW bass | float | 0 | 1.180.711 | audio |
| ch2_ch_adjust | CH. ADJUST | float | 0 | 1.180.714 | audio |
