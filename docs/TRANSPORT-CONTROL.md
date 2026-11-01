# PLAY / STOP — versão 0.2.1

Antes: os botões eram indicadores do estado do host e não executavam ações. Agora, clicar em PLAY ou STOP solicita a mudança de transporte pela tecla Espaço da janela da DAW no Windows. As luzes, o contador e os carretéis continuam acompanhando o estado informado pelo host; não simulamos uma parada local ou silenciamos o áudio para fingir que a DAW parou.

## Uso

- Atualize a pasta VST3 inteira com a DAW fechada.
- Em SETTINGS, mantenha habilitado **PLAY / STOP via tecla Espaço da DAW (Windows)**. O padrão é habilitado; o ajuste é salvo na sessão.
- Confirme que Espaço inicia/para a reprodução quando a janela principal da DAW está ativa. Se esse atalho foi personalizado, restaure-o para usar estes botões.
- As rotas implementadas identificam os processos Windows `Studio One.exe` / `StudioOne.exe` e `reaper.exe` / `reaper64.exe`. Processos de bridge separados ou outros hosts não são suportados por esse mecanismo.
- REC continua sendo indicador; esta alteração não inicia gravação.

## Implementação e limites

A API VST3/JUCE usada não fornece controle de transporte para essa instância de playhead. A extensão PreSonus IHostCommandHandler disponível no SDK é para comandos de menus de parâmetros, não uma API geral de Start/Stop. Não foi inventada uma interface de transporte do Studio One.

O helper executa na thread de interface. Ele identifica a janela da DAW no processo atual e só envia o par WM_KEYDOWN/WM_KEYUP de Espaço quando o host está ativo. Não usa SendInput, não injeta teclas globalmente e não envia comandos a outro processo. Leva o foco à janela do host para o comando não virar entrada de texto no plugin.

Como Espaço é um toggle, uma política compara a ação desejada com o estado observado e mantém solicitações pendentes. Isso evita que dois cliques PLAY, antes da confirmação do host, alternem para Stop. A política aceita STOP depois de PLAY pendente e limpa pendências confirmadas. Após dois segundos sem confirmação, o status em SETTINGS orienta conferir o atalho. Teclas customizadas, foco bloqueado ou estado do host desatualizado podem impedir a ação correta; não é uma API nativa universal de transporte.

O plugin não consulta getPlayHead/getPosition pela thread de interface. O áudio publica o estado por atomics, com revisão em mudanças de reprodução. Não há comandos de teclado na thread de áudio, durante leitura de estado ou carregamento de presets.

## Evidência

- Testes de política de comandos: PLAY/STOP, clique repetido, confirmação atrasada, rota indisponível e timeout.
- Teste Win32 real sob Wine em processo/janela simulados: envio do espaço, PLAY seguido de STOP e repetição PLAY sem segunda alternância. O executável de teste é nomeado para exercitar a identificação do host e não é Studio One real; não é incluído no ZIP de uso.
- Testes do editor: callbacks ligados, preservação do estado observado quando uma rota não é suportada, persistência da opção e demais controles.
- Ainda é necessário validar a interpretação do atalho pelo Studio One Pro 8 instalado pelo usuário. Não se declara esse teste como executado neste ambiente.
