# Validação — PLAY/STOP 0.2.1

A alteração liga os botões ao atalho Espaço da janela da DAW no Windows. Não é uma API nativa VST3 de transporte e exige o atalho de reprodução/parada padrão. Consulte TRANSPORT-CONTROL.md.

| Verificação | Resultado |
|---|---|
| Build VST3 Windows x64 / JUCE 7.0.12 / MinGW-w64 | Passou, versão 0.2.1 confirmada pelo pluginval |
| CTest nativo Linux | 3 de 3 passaram: níveis, política de transporte, processador/estado |
| Política de PLAY/STOP | Passou: clique repetido, confirmação atrasada, solicitação oposta pendente, indisponibilidade e timeout |
| Mensagens Win32 em janela simulada sob Wine | Passou: PLAY seguido de STOP; repetir PLAY não alterna duas vezes |
| Editor final Linux / Xvfb | Passou: abrir/fechar 3x, callbacks de transporte sem simular estado, knobs, seletores, abas, busca e resize |
| Configuração do transporte salva/restaurada | Passou no teste do processador; permanece como propriedade de software, sem novo parâmetro de áudio |
| pluginval 1.0.4 strictness 5 sem GUI, binário Windows 0.2.1 sob Wine | SUCCESS, exit 0, até 192 kHz |
| Dois testes Python da matriz de 90 presets | Passaram |
| Studio One Pro 8 real / atalhos do usuário | Não executado neste ambiente; ainda requer validação na instalação do usuário |
| pluginval completo com GUI 0.2.1 | Não executado; a execução 0.2.0 teve timeout no Editor sob Wine, registrado em VALIDATION-0.2.0.md |
| Steinberg validator separado | Não executado; ferramenta não fornecida |

O teste de integração Windows foi realizado em processo/janela simulados, deliberadamente nomeados para exercitar a identificação do host. Ele valida o envio e recebimento Win32; não é Studio One e não comprova como o Studio One real interpreta o atalho. Esse executável de teste não é distribuído no ZIP VST3.

O pluginval sem GUI usou 44100/48000/96000/192000 Hz, blocos 64/128/256/512/1024 e seed 12345. Concluiu processamento, estado, automação, programas e buses. Nenhuma aprovação completa de GUI é reivindicada.

O helper usa WM_KEYDOWN/WM_KEYUP apenas na janela identificada do processo atual. Não utiliza SendInput global. Nunca lê getPlayHead/getPosition na interface: a thread de áudio publica os estados por atomics. Carregar uma sessão ou preset não envia comandos. REC continua indicador.

Logs no ZIP: transport-021-audio.log, transport-021-win32-final.log, transport-021-ctest.log e transport-021-editor.log. Os IDs VST3/64 parâmetros e schema 1 permanecem compatíveis; transport_shortcut_enabled é uma propriedade opcional, padrão habilitado.

O processamento de fita, EQ, bias e saturação permanece pendente; esta versão corrige a ação dos controles de transporte e não conclui o modelo analógico.
