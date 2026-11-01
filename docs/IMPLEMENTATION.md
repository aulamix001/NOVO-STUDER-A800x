# Implementação 0.3.0

O DSP de fita, a tabela completa de controles e suas limitações estão em [TAPE-DSP.md](TAPE-DSP.md). Os controles antigos de áudio agora alimentam o modelo de fita; os 64 IDs anteriores foram preservados e seis parâmetros foram acrescentados ao final. Os estados antigos recebem valores padrão para os novos parâmetros.

O modelo aproxima efeitos de fita por filtros e saturação com memória, sem medições da A800. A validação automatizada verifica software e interface, não equivalência física. PLAY/STOP permanecem limitados aos hosts Windows suportados; REC controla o efeito e MEMO controla a referência do contador.
