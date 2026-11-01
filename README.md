# A800 — VST3 estéreo / interface visual 0.2.0

Este projeto ainda **não emula o som de fita da A800**. Implementa infraestrutura JUCE, 64 parâmetros versionados, ganhos por canal com suavização, seleção INPUT/REPRO/SYNC em nível de roteamento/ganho, MASTERSAFE limpo, persistência de estado e 90 programas de alinhamento. A interface MACHINE agora utiliza a própria arte estéreo de referência, com knobs, seletores, medidores e contador interativos. [Detalhes da interface](docs/VISUAL-INTERFACE.md).

Os controles `[PENDING]` no host guardam estado mas não implementam o DSP completo: EQ, seleção da formulação da fita, bias, treble/bass, varispeed, calibração de saída e stereo link. A interface visual limpa esses sufixos das etiquetas; SETTINGS e tooltips explicam o estado de implementação. VUs, contador e indicadores do host agora funcionam como medição/animação de software. `[VISUAL]` não tem DSP por especificação. Machine type permanece como configuração; Speed seleciona apenas os trimpots Level ativos. Não há oversampling ou latência porque não há estágio não linear implementado.

INPUT usa entrada com Input Level e sensibilidade; REPRO e SYNC têm saídas de ganho separadas. O ponto de tap INPUT ainda precisa ser conferido no manual. MASTERSAFE passa sinal original sem ganho. Os ganhos têm rampa de 20 ms, uma escolha de software.

## Windows 10/11 x64 — build MSVC 2022

Instale Visual Studio 2022 com Desktop development with C++, Windows SDK, CMake, Git e Python 3. Execute em Developer PowerShell:

```powershell
python tools/generate_presets.py
python tools/generate_parameters.py
python tests/test_presets.py
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

O CMake busca JUCE 7.0.12 no commit fixado. Para checkout existente use `-DJUCE_SOURCE_DIR=C:/caminho/JUCE`. A pasta gerada é `build/A800Prototype_artefacts/Release/VST3/A800 Technical Prototype.vst3`. Copie a pasta inteira para `%COMMONPROGRAMFILES%\VST3\` e faça nova varredura na DAW. Ainda requer teste numa DAW Windows real.

`.github/workflows/windows.yml` prepara o build e testes MSVC em GitHub Actions quando o código for publicado; não foi executado nesta sessão.

## Presets

90 alinhamentos de fábrica: 15 fitas × NAB/CCIR × 7,5/15/30 ips. Seleção pelo editor ou programas do host; estado salvo na sessão. [Detalhes e limites](docs/PRESETS.md). Estes presets **não são 90 timbres**: como o modelo de fita/EQ não existe, ficam sonoramente iguais em posição nominal.

## Fontes e validação

- [Tabela de parâmetros](docs/PARAMETERS.md), contrato JSON em `config/parameters.v1.json`.
- [Roteamento pretendido](docs/AUDIO-MAPPING.md) e [dados pendentes](docs/TODO-MANUAL.md).
- [Estado da implementação](docs/IMPLEMENTATION.md).
- Testes de níveis/velocidades: `tests/mapping_tests.cpp`.
- Testes do processador/estado/programas: `tests/processor_tests.cpp`.
- Testes de presets/tabela: `python tests/test_presets.py`.

O texto anexado é a fonte disponível. Os manuais SI 66/81 E e A800 Audio Adjustments e medições de referência não foram fornecidos. Não se declara fidelidade sonora ao hardware.
