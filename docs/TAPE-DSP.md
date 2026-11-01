# Modelo de fita 0.3.0

## Caminho de áudio

Entrada → sensibilidade/ganho de gravação → EQ de gravação → ênfase → saturação com memória aproximada em 4x → hiss → deênfase → head bump → EQ de reprodução → perda de agudos → modulação opcional → resíduo de apagamento aproximado → bloqueio de DC → ganho de saída → mistura wet/dry.

A saturação usa tanh com pequena assimetria dependente do perfil e um estado magnético de primeira ordem. Não é um solucionador Jiles–Atherton nem um modelo físico medido. A ênfase/deênfase é aproximada por shelves complementares, com frequências derivadas das constantes de tempo selecionadas por padrão e velocidade. Os filtros IIR são causais: não há lookahead nem latência fixa declarada ao host. Há resposta de fase dos filtros e, quando ativado, atraso intencional do efeito mecânico. Oversampling reduz aliasing; não o elimina.

As curvas dos 15 perfis em `src/dsp/TapeModel.h` são escolhas de projeto para headroom, largura de banda, head bump, memória e assimetria, sem medições das formulações nomeadas. O hiss é pseudoaleatório e independente por canal; o movimento mecânico é coerente entre os canais. O silêncio é preservado se hiss estiver em -100 dBFS, salvo o decaimento dos estados dos filtros.

## Mapeamento completo

| Controle | Ação |
|---|---|
| Machine type / SPEED | Selecionam 3,75/7,5/15/30 ips e o banco FAST/SLOW |
| Tape | Seleciona um dos 15 perfis aproximados |
| EQ NAB/CCIR | Ênfase/deênfase e compensação tonal |
| INPUT / REPRO / SYNC | Monitoração linear / caminho de fita / reprodução com largura de banda de sincronismo |
| Record Input / sensibilidade | Ganho antes da fita; +7 dB aumenta a excitação |
| REC FAST/SLOW Level / Treble | Ganho e shelf de gravação do banco ativo |
| REP FAST/SLOW Level / Treble / Bass | Ganho e shelves do caminho REPRO |
| SYN FAST/SLOW Level / Treble / Bass | Ganho e shelves do caminho SYNC |
| Output 1 / Output 2 | Ganhos depois do modelo para REPRO / SYNC |
| BIAS FAST/SLOW / CH. ADJUST | Bias e ajuste fino: alteram excitação, agudos e saturação |
| ERASE FAST/SLOW | Nível de resíduo aproximado com tap de 20 ms; efeito sutil, não simula gravação física em loop |
| Oscillator stop | Estado sem bias: mais distorção/assimetria e resíduo |
| Sync range | Largura de banda wide/narrow, somente no caminho SYNC |
| Lower speed treble boost time/amount | Frequência e quantidade de compensação, apenas em SLOW; posição 0 desliga |
| Repro input external | Desvia o estágio magnético; mantém o caminho de eletrônica/reprodução |
| Alignment dBFS | Referência de excitação da fita e dos medidores |
| VU jumper / CALIBRATE | Referência de linha dos VUs / medição de saída; não geram tom de teste |
| Stereo link | Espelha edições L/R feitas na interface; não combina canais de áudio ou automação |
| VARISPEED / semitones | Varia a velocidade efetiva do modelo por razão 2^(semitons/12), alterando o timbre; não transpõe o fluxo ao vivo |
| Transport modelling / Wow / Flutter | Variação de atraso com componentes de 0,57 Hz e 8,7 Hz; atraso base de 6 ms |
| Tape drive | Excitação adicional de -12 a +12 dB, com normalização de ganho de pequeno sinal |
| Tape wet mix | Mistura do original com o caminho de fita, 0–100% |
| Tape hiss | Ruído entre -100 (desligado) e -50 dBFS |
| REC | Liga/desliga o caminho de fita; não grava arquivos nem arma pistas |
| MASTERSAFE | Passagem exata do sinal original |
| ZERO LOC / MEMO | Zera / guarda e restaura a referência do contador; sem mover a DAW |
| PLAY / STOP | Integração por atalho Espaço no Studio One/REAPER Windows; demais hosts precisam de suporte específico |
| MACHINE / SERVICE / PRESETS / SETTINGS | Navegação; busca, carregar e anterior/próximo selecionam os 90 programas |

Os trimpots dos bancos inativos continuam automatizáveis e salvos; passam a atuar quando a velocidade/rota correspondente é selecionada. A mistura paralela pode ter coloração de fase devido aos filtros/atrasos. Mudanças discretas de rota e de coeficientes podem produzir transientes; ganhos, drive e mix têm rampa de 20 ms.

## Validação e limites

Os testes verificam compressão não linear, diferenças entre perfis, comportamento dos controles, estabilidade em quatro taxas de amostragem, independência do tamanho de bloco, mistura seca, medição, restauração de estados e interface. Nenhum desses testes prova fidelidade à A800. Comparação de resposta, THD/harmônicos, ruído e wow/flutter com medições do hardware e audição em uma DAW Windows real continuam necessárias.
