# Contrato de áudio e painel

Prioridade: validar o caminho de áudio antes de investir na animação do painel. Este documento descreve o alvo completo. Agora existe um VST3 técnico parcial; consultar IMPLEMENTATION.md antes de interpretar qualquer controle como implementado. Não há alegação de equivalência sonora ao hardware.

## Roteamento

Por canal: entrada → Input Level / sensibilidade +7 dB → Record Level da velocidade ativa → Record EQ → fita → dois caminhos independentes de leitura.

- REPRO: cabeça de reprodução → Reproduce EQ e Level/Treble/Bass ativos → Output 1 Level.
- SYNC: cabeça de gravação em leitura → Sync EQ e Level/Treble/Bass ativos → Output 2 Level. Não encadear SYNC depois de REPRO.
- INPUT: monitoração de entrada sem fita. O ponto exato do tap em relação ao Input Level precisa do diagrama original.
- MASTERSAFE: caminho de entrada limpo conforme o texto fornecido; conferir o comportamento no manual antes da implementação.

Somente Fast ou Slow da versão selecionada atua. Os ajustes da outra velocidade permanecem salvos. O boost de velocidade baixa só atua em Slow; valores 0/C/B/A e 1–4 não serão convertidos em frequências ou ganhos arbitrários.

Bias efetivo = tabela(fita, velocidade) + bias_fast ou bias_slow + chN_ch_adjust. É uma medida de queda em 10 kHz em relação ao pico, não um ganho direto do sinal. A tabela não fornece as curvas magnéticas de cada formulação. Não derivar saturação ou assinatura de uma fita apenas desse número.

NAB/CCIR exige pré/de-ênfase coerente ao redor da não linearidade. Frequências de medição (50 Hz, 700 Hz/1 kHz, 10/12 kHz) não são frequências de corte de filtros. As constantes de tempo do texto estão marcadas como não verificadas; não há filtros implementados.

## Painel frontal

| Elemento | Ligação / comportamento |
|---|---|
| VU 1 e 2 | Medição L/R; OPERATE e CALIBRATE conforme ponto de medição a conferir |
| VU 3–24 | Sem sinal e identificados como inativos no plugin estéreo |
| INPUT/SYNC/REPRO 1 e 2 | ch1_src e ch2_src |
| INPUT/SYNC/REPRO 3–24 | Sem ligação de áudio; não duplicar L/R para simular 24 pistas |
| READY | Indicação; sem parâmetro sonoro fictício; relação com REC depende de confirmação no manual |
| PLAY/STOP/REC/PAUSE | PLAY/STOP: a partir de 0.2.1, atalho Espaço direcionado à DAW no Windows; REC/PAUSE continuam indicação. Não condiciona o DSP do insert |
| FF/REW/LOCATE/EDIT | Representação visual; não reposiciona o host nem altera o áudio |
| Contador | Tempo do host; ZERO LOC armazena uma referência local |
| ERASE / oscillator stop / repro external | Estado visual persistido, sem efeito de áudio conforme especificação fornecida |
| Carretéis | Animação do transporte, sem controle de saturação ou velocidade fictício |

Configurações 8/16/24 canais, largura de fita, alimentação, canal de código e S3 não criam parâmetros DSP. HF Driver Y REC deve constar como configuração visual de serviço; sua semântica e opções exatas aguardam o manual, sem ID automatizável fixado nesta versão.

Stereo link deve ser uma operação explícita da UI sobre os dois parâmetros, nunca escrever parâmetros dentro de processBlock. Com link desligado, automação L/R permanece independente. Ranges provisórios estão em TapeConstants.h.

## Critérios para o DSP futuro

1. Referência digital padrão: −18 dBFS corresponde a 0 VU / 320 nWb/m. Fluxo é proporcional à amplitude: +6 dB dá aproximadamente 638,5 nWb/m; a duplicação exata ocorre em +6,0206 dB. Não distorcer a lei de dB para obter 640 exatos em +6,000 dB.
2. Medir resposta de baixo nível por velocidade/padrão e SYNC wide/narrow. A posição calibrada não basta para comprovar essas respostas.
3. Medir curvas entrada/saída, harmônicos por nível/frequência, compressão de HF, resposta transitória e comportamento de bias com gravações ou medições do hardware. O manual de alinhamento sozinho não determina uma emulação magnética única.
4. Processamento estéreo 44,1–192 kHz; oversampling 2x/4x com filtragem antialias e latência reportada. Sem alocações, locks ou operações de estado na thread de áudio; parâmetros suavizados e denormais tratados.
5. Wow/flutter: percentuais DIN ponderados não equivalem diretamente à profundidade de um LFO. Exigir medição com ponderação antes de declarar conformidade. Ruído e transporte ficam desativados até calibração.
6. Varispeed de insert é limitado por armazenamento finito: uma diferença de velocidade sustentada consome buffer. Validar recentragem, pitch e latência antes de habilitar.
7. Validar estado/automação, presets versionados e pluginval strictness ≥5 no VST3 Windows. Esses testes ainda não foram executados; não existe binário.
