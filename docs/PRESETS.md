# Presets de alinhamento

90 configurações: 15 formulações × 3 velocidades (7,5 / 15 / 30 ips) × 2 padrões (NAB / CCIR). Fonte única: tabela e instruções do texto anexado pelo usuário, não manual original conferido ou recomendações de sites.

Todos os trimpots e offsets de bias ficam em zero, posição calibrada. O valor absoluto de overbias é metadado derivado da tabela por fita/velocidade, não deve ser aplicado como ganho nem duplicado no offset. 0 VU corresponde ao alinhamento digital −18 dBFS e ao fluxo operacional 320 nWb/m.

Formato: JSON esquema 1, IDs do contrato v1, sem valores secretos ou dependências de caminho absoluto. Cada arquivo contém nome, categoria, fonte, fita, EQ, velocidade e parâmetros. `tools/generate_presets.py` regenera os arquivos; `tools/generate_parameters.py` gera as declarações APVTS e programas embutidos. Ambos devem executar antes do build se houver alteração na tabela.

No protótipo técnico, a lista aparece no editor e como 90 programas do host, com persistência no estado da sessão. A versão visual 0.2.0 adiciona busca, carregamento e anterior/próximo. Não há importação/exportação interativa de presets de usuário, A/B ou gerenciamento de pastas ainda. Os JSON são entregues como dados para essa implementação posterior.

Importante: os presets são alinhamentos, não 90 timbres já modelados. Como fita, NAB/CCIR e bias ainda não têm DSP, estas combinações não produzem diferenças sonoras na posição nominal. Não há evidência para fabricar presets 'warm', 'vintage', de artista ou estúdio e atribuí-los ao hardware.
