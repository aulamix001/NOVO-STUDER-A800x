# Evidência pendente

Fonte disponível: Texto colado.txt fornecido pelo usuário. Fontes ausentes: SI 66/81 E e A800 Audio Adjustments, diagramas e medições de áudio.

- Conferir cada parâmetro/tabela com página, edição e cartão do manual; nenhuma entrada tem status de especificação oficial validada.
- Confirmar pontos de monitoração INPUT e medição OPERATE/CALIBRATE, MASTERSAFE, READY e caminho SYNC.
- Confirmar ranges dos trimpots, bias global/fino, jumpers Y REC e varispeed.
- Conferir NAB/IEC por versão/velocidade; não usar 50 Hz/10 kHz/12 kHz como cortes de EQ.
- Resolver ambiguidade da tolerância a 7,5 ips e falta de resposta especificada a 3,75 ips.
- Obter curvas magnéticas/THD, perdas de cabeça, calibração de bias por fita, ruído e wow/flutter ponderado. Não preencher com valores de outras máquinas.
- Balística VU de 300 ms é provisória. Default REPRO e transporte desativado são decisões de software pendentes de validação.
- Faixas provisórias ficam em src/dsp/TapeConstants.h, com TODO-MANUAL. Saturação, histerese, head bump e filtros não foram implementados.
- Sem DSP para erase, oscillator stop, repro external; sem comandos de busca no host; sem 24 canais reais no insert estéreo.
