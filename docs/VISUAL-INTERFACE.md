# Interface visual 0.2.0

A tela MACHINE utiliza a própria arte da referência estéreo, incorporada ao VST3 em alta resolução. Moldura, alumínio, serigrafia, parafusos, carretéis, fita, roletes e cabeças são preservados na imagem de fundo. Os controles sobrepostos e os elementos dinâmicos são renderizados pelo JUCE; posições das agulhas, luzes, números e knobs variam com o estado. Não se promete igualdade de pixels para esses elementos móveis.

- Dois VUs com leitura RMS de entrada/saída e balística aproximada; CALIBRATE seleciona a medição de saída. Não gera sinal e não implementa a calibração de saída −3 dBu do hardware. A referência segue o alinhamento digital configurado, sem certificação VU/DIN.
- Contador acompanha segundos do host, com referência zero pelo botão RESET; essa referência é salva no estado. MEMO é um detalhe visual sem comando no host.
- Carretéis animam quando o host indica reprodução. A animação não é uma simulação física de tensão/diâmetro da fita.
- STOP / PLAY / REC indicam o estado da DAW e não comandam o transporte.
- INPUT LEVEL, OUTPUT 1 e INPUT/REPRO/SYNC por canal têm attachments aos IDs existentes. Duplicar clique restaura defaults; arrasto vertical, roda e Shift permitem ajuste.
- Machine type, Slow/Fast, NAB/CCIR, Tape, CALIBRATE e MASTERSAFE aparecem nas posições da referência. Tape e EQ ainda não têm DSP; seleção é salva e restaurada.
- SERVICE mantém os demais ajustes acessíveis; SETTINGS explica os limites da implementação e contém os controles de software. A tela MACHINE não contém o editor técnico ou avisos sobrepostos à imagem.
- PRESETS oferece busca dos 90 alinhamentos, seleção, carregamento, anterior/próximo e duplo clique. Não adiciona presets de timbre fictícios.
- Dimensão inicial 1152 × 768; redimensionamento com proporção 3:2, de 768 × 512 a 3072 × 2048. Arte embutida; não depende de arquivo de imagem externo.

IDs/version hints dos 64 parâmetros e identificador do plugin foram preservados. A engine continua um protótipo de ganhos e roteamento: não houve implementação de saturação, curvas de fita, EQ, bias ou flutter nesta atualização visual.
