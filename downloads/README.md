# Download — A800 / PLAY e STOP 0.2.1

**[Baixar VST3 Windows x64 — versão 0.2.1](./A800-Transport-0.2.1-Windows-x64.zip?raw=true)**

[Baixar código-fonte](./A800-Technical-Prototype-Source.zip?raw=true) · [SHA256](./SHA256SUMS.txt)

![Interface da versão 0.2.1](./interface-0.2.1.png)

## PLAY / STOP agora enviam comandos

Antes eram apenas indicadores. Agora os cliques enviam a tecla Espaço à janela da DAW no Windows para solicitar reprodução/parada. As luzes continuam acompanhando o estado real do host.

Em **SETTINGS**, mantenha habilitado **PLAY / STOP via tecla Espaço da DAW (Windows)**. É necessário que Espaço esteja configurado para reproduzir/parar o transporte no Studio One. A rota também identifica o REAPER nativo; outros hosts/processos de bridge não são suportados por esse mecanismo.

- Cliques repetidos em PLAY não enviam nova alternância enquanto a primeira solicitação está pendente.
- O comando é enviado pela interface mesmo quando o áudio está parado, sem depender de um próximo bloco de áudio.
- Não há injeção global de teclado: as mensagens são direcionadas à janela identificada da DAW no próprio processo.
- REC continua indicador de gravação, sem comando de gravação.

## Atualizar

1. Feche o Studio One / DAW.
2. Baixe e extraia o ZIP **0.2.1** acima.
3. Substitua a pasta inteira `A800 Technical Prototype.vst3` em `C:\Program Files\Common Files\VST3\`. Evite outra cópia antiga em uma pasta adicional de plugins.
4. Abra a DAW e faça nova busca de VST3. Confira a versão **0.2.1** na aba SETTINGS.
5. Confira se Espaço inicia/para a reprodução na janela principal da DAW e teste os botões no plugin.

## Evidência e limites

O helper de transporte passou num teste de mensagens Win32 reais com uma janela de host simulada sob Wine. Os três testes CTest e os testes de interação do editor passaram; pluginval strictness 5 sem GUI passou no binário Windows 0.2.1 até 192 kHz. Não houve teste em Studio One Pro 8 real neste ambiente Linux. O teste simulado não certifica os atalhos da sua instalação.

O último teste completo de GUI no Wine (0.2.0) teve timeout; a interface direta Linux passou. Relatório e logs estão no ZIP. [Detalhes do transporte](../docs/TRANSPORT-CONTROL.md).

O painel realista, os 90 alinhamentos e os parâmetros anteriores foram preservados. O DSP de fita, saturação, EQ e bias ainda está pendente; os presets ainda não são 90 timbres distintos.
