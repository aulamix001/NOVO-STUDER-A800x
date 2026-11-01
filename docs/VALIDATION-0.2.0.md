# Validação — interface visual 0.2.0

Esta versão substitui o editor técnico por um painel com a arte estéreo fornecida como referência. O motor de áudio continua parcial: não implementa saturação, curvas de fita, EQ, bias ou flutter.

| Verificação | Resultado |
|---|---|
| Build Windows x64, JUCE 7.0.12 / MinGW-w64 | Passou; metadados VST3 confirmam 0.2.0 |
| Testes C++ de níveis, velocidades, 64 parâmetros, 90 programas, estado e ganhos L/R | Passou no Linux / JUCE 8.0.6 |
| Medição RMS e MASTERSAFE | Passou nos testes do processador |
| Abrir/fechar o novo editor três vezes | Passou no teste direto Linux / Xvfb |
| Knob INPUT LEVEL e seletor Tape ligados ao estado | Passou no teste de interação |
| Abas MACHINE / SERVICE / PRESETS / SETTINGS | Passou no teste de interação |
| Busca AMPEX 456 retorna seis alinhamentos; seleção/carregamento | Passou no teste de interação |
| Redimensionamento e captura renderizada da interface | Passou; captura disponível em downloads/interface-0.2.0.png |
| Dois testes Python da matriz de presets e tabela | Passou |
| pluginval 1.0.4 strictness 5, binário Windows 0.2.0 sob Wine 10, sem GUI | SUCCESS, exit 0 |
| pluginval completo sob Wine 10 | Falhou por timeout de 30 s em Editor, como na versão anterior |
| Steinberg validator separado | Não executado; pluginval informa skip por falta do executável |
| DAW Windows real / Studio One Pro 8 | Não executado neste ambiente Linux |

O pluginval sem GUI usou sample rates 44100/48000/96000/192000 Hz, blocos 64/128/256/512/1024 e seed 12345. Foram concluídos processamento, estado, automação, 90 programas e buses. Os testes de GUI foram omitidos explicitamente nessa execução, portanto não se declara aprovação completa strictness 5.

O novo editor não contém um GenericAudioProcessorEditor aninhado. A tela usa a imagem embutida mais controles próprios. O teste direto Linux abriu e fechou o editor, operou controles/abas e gerou um screenshot real. Isso não substitui o teste do binário na DAW Windows: o timeout observado sob Wine ainda não tem causa isolada.

Os identificadores VST3 e os IDs/version hints dos 64 parâmetros foram mantidos. O estado mantém schema 1; foi acrescentada a propriedade opcional zero_reference para o contador. A balística VU é aproximada; CALIBRATE seleciona a medição de saída e não implementa a calibração −3 dBu do hardware.

Os logs desta versão estão em logs/visual-020-audio.log e logs/visual-020-full.log dentro do ZIP. Resultados antigos permanecem no histórico docs/VALIDATION-0.1.0.md.
