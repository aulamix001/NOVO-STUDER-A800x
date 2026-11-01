# A800 Tape DSP — 0.3.0

**[Baixar VST3 Windows x64 — 0.3.0](downloads/A800-Tape-0.3.0-Windows-x64.zip?raw=true)** · [Como instalar](downloads/README.md)

Plugin VST3 estéreo com processamento de fita: saturação suave dependente do nível, memória magnética aproximada, ênfase/deênfase de gravação/reprodução, head bump, perda de agudos, bias, hiss e wow/flutter. O estágio não linear trabalha internamente em 4x com filtros IIR de interpolação/decimação.

É um modelo de engenharia inspirado em gravadores de fita, **sem calibração por medições de uma Studer A800 real**. Os 15 perfis nomeados são aproximações; não representam curvas medidas das formulações. Não se afirma equivalência sonora ao hardware nem aos plugins comerciais. [Modelo e controles](docs/TAPE-DSP.md).

## Uso

- Use REPRO para o efeito completo. INPUT monitora a entrada com ganho e sensibilidade; SYNC usa a equalização e a largura de banda de sincronismo.
- Aumente Tape drive em TAPE FX (SETTINGS) para mais saturação. Tape wet mix permite mistura paralela; 100% é a posição recomendada para evitar interferência de fase entre caminhos.
- Machine type e SPEED selecionam a velocidade efetiva, a resposta espectral e os trimpots FAST/SLOW em SERVICE. VARISPEED modifica a velocidade **tonal**; não é um transpositor contínuo de áudio ao vivo.
- REC liga/desliga o processamento de fita deste plugin. MASTERSAFE passa o sinal original. REC não arma a gravação da DAW.
- TRANSPORT MODELLING habilita wow/flutter, com aproximadamente 6 ms de atraso intencional de efeito. Não depende de apertar PLAY para processar áudio.
- Stereo link vincula os controles L/R editados na interface. Automação da DAW continua independente por canal.
- PLAY/STOP usam o atalho Espaço no Studio One/REAPER Windows. Não há uma API VST3 universal para comandar o transporte. ZERO LOC redefine a referência visual do contador; MEMO guarda/restaura essa referência, sem mover o cursor da DAW.

## Compilar e testar

JUCE 7.0.12 é fixado no commit `4f43011b96eb0636104cb3e433894cda98243626`. Requer CMake >= 3.22, C++17 e Python 3. No Linux, instale os pacotes de desenvolvimento ALSA, FreeType, X11/Xext/Xinerama/Xrandr/Xcursor e OpenGL, Xcomposite, Xvfb, xauth e Openbox para os testes gráficos. WebKit e libcurl não são necessários.

```sh
python3 tests/test_presets.py
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
xvfb-run -a sh -c 'openbox >/tmp/a800-openbox.log 2>&1 & build/processor_tests_artefacts/Release/processor_tests --gui'
```

No Windows, use MSVC 2022 x64 e `cmake --build build --config Release --parallel 2`, seguido de `ctest --test-dir build -C Release --output-on-failure`. O produto mantém o nome `A800 Technical Prototype.vst3` para preservar a identidade do plugin.

O ambiente na nuvem usa o contêiner `a800-dev`, fonte `/src`, build nativo `/build` e build Windows `/build-win`. A compilação Windows usa `cmake/mingw.cmake`, `A800_MINGW_ROOT=/usr`, `JUCE_SOURCE_DIR` e o `JUCE_HOST_HELPER` nativo; o executável do JUCE nativo não pode ser substituído por um executável Windows na compilação cruzada.

## Compatibilidade

Os primeiros 64 IDs, ordem, versão de parâmetro e IDs VST3 foram mantidos. Seis parâmetros foram acrescentados ao final: drive, mix, hiss, wow, flutter e REC. Estados antigos schema 1 com 64 parâmetros recebem os novos valores padrão; estados novos usam schema 2. A restauração dos valores antigos não preserva o som linear da versão 0.2.1, pois o processamento de fita agora está ativo por padrão.

Os 90 programas continuam alinhamentos de 15 perfis × NAB/CCIR × 7,5/15/30 ips. Os geradores `tools/generate_presets.py` e `tools/generate_parameters.py` atualizam arquivos versionados e só devem ser usados ao alterar deliberadamente o contrato de parâmetros.
