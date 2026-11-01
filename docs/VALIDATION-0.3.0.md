# Validação A800 Tape DSP 0.3.0

- Build Linux Release: passou.
- Build Windows x64 Release / MinGW / JUCE 7.0.12: passou.
- CTest: 4/4 passaram (tape_tests, mapping_tests, transport_tests, processor_tests).
- Python: 2/2 testes de presets passaram.
- Auditoria APVTS: todos os parâmetros de áudio alteram a rota ativa; o banco de velocidade inativo não altera o áudio.
- DSP: 44,1/48/96/192 kHz, compressão de sinais fortes, estabilidade, 15 perfis, EQ, bias, resíduo, hiss, wow/flutter, mistura seca e independência do tamanho de bloco.
- Estado: 70 parâmetros, 90 programas, restauração, rejeição de estado inválido e migração do schema 1 com os 64 parâmetros antigos.
- GUI Linux com Xvfb/Openbox: abrir/fechar três vezes, abas, busca, presets, controles, stereo link, REC, MEMO e redimensionamento; capturas reais incluídas.
- pluginval Windows 1.0.4 sob Wine: nível 5, sem GUI, seed 12345, 44100/48000/96000/192000 Hz, blocos 64/128/256/512/1024. SUCCESS / exit 0.
- Exportações Windows GetPluginFactory, InitDll e ExitDll presentes. Sem dependência dinâmica de libgcc/libstdc++/libwinpthread.

## Não realizados

Não houve comparação por medições ou audição contra hardware A800, nem validação numa DAW Windows real. A GUI foi testada no Linux; os testes de GUI do pluginval Windows foram omitidos. Não houve validação em MSVC, macOS ou Steinberg validator separado.

## Limites funcionais

VARISPEED modifica a velocidade tonal, não faz transposição contínua ao vivo. PLAY/STOP dependem de integração específica com Studio One/REAPER Windows. REC ativa o efeito, sem armar pistas. MEMO opera sobre a referência do contador. O modelo é uma aproximação de engenharia; não se declara equivalência sonora ao hardware. Consulte MANUAL.md.
