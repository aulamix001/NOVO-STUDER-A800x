# Validação — 2026-11-01

Entrega: A800 Technical Prototype 0.1.0. Não é emulação analógica concluída, nem aprovação para produção.

| Verificação | Resultado |
|---|---|
| Testes C++ de fluxo/VU, calibração e velocidades | Passou |
| Testes JUCE de 64 parâmetros, 90 programas, quatro sample rates, ganhos L/R, estado e MASTERSAFE | Passou no Linux com JUCE 8.0.6 |
| Teste direto de criação, redimensionamento e exibição do editor | Passou no Linux / Xvfb |
| Dois testes Python da matriz de 90 presets, parâmetros e tabela de bias | Passou |
| Compilação VST3 Windows x64 | Passou com JUCE 7.0.12 / MinGW-w64 GCC 14-posix |
| Inspeção PE e entry points VST3 | PE x86-64; GetPluginFactory, InitDll, ExitDll presentes |
| pluginval v1.0.4, strictness 5, sem GUI, binário Windows sob Wine 10 | SUCCESS, exit 0 |
| pluginval completo, binário Windows sob Wine 10 | Falhou por timeout de 30 s em Editor; repetição com fontes instaladas também falhou no mesmo teste |
| Steinberg VST3 validator | Não executado; o pluginval informa skip por falta do executável |
| Build MSVC / testes Windows nativos / DAW Windows real | Não executados neste ambiente Linux |
| Fidelidade ao hardware, resposta EQ, saturação, bias, flutter | Não validado; DSP correspondente não implementado |

Pluginval sem GUI: sample rates 44100, 48000, 96000, 192000 Hz; blocos 64, 128, 256, 512, 1024; seed 12345. Testes de processamento, estado, automação, programas e buses concluíram. Testes de GUI foram explicitamente omitidos nessa execução; não representa aprovação completa strictness 5.

A falha de interface no Wine ainda não permite concluir se o problema é exclusivo do runtime ou também se manifesta no Windows real. O teste nativo Linux passou, mas usa outra plataforma/framework e não elimina o risco no binário Windows. Não desativamos o teste na configuração Windows CI.

JUCE 8 recusou MinGW por falta de suporte; não alteramos o framework. A compilação Windows utilizou JUCE 7.0.12 (commit 4f43011b96eb0636104cb3e433894cda98243626), compatível com a stack solicitada. O juceaide nativo 8.0.6 foi usado como gerador de recursos; não foi vinculado ao plugin Windows. O binário importa apenas DLLs de sistema Windows, sem dependências externas MinGW/libstdc++.

Os manuais originais e medições do hardware continuam ausentes. Os 90 presets são configurações de alinhamento derivadas do texto anexado, sem 90 timbres distintos. O painel fotográfico é apenas conceito visual; o editor entregue é técnico.

O workflow `.github/workflows/windows.yml` prepara compilação MSVC, testes e pluginval completo em Windows. Ele foi salvo no disco, mas não foi publicado nem executado.
