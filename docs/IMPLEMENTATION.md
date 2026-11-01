# Estado da implementação

| Área | Implementado | Pendente |
|---|---|---|
| Host | VST3 estéreo; 64 parâmetros IDs v1 | Validação em DAW Windows real |
| Áudio | Ganhos L/R, Level Fast/Slow, rotas INPUT/REPRO/SYNC, MASTERSAFE; rampa de ganhos | Fita, bias, EQ, perda de cabeças, SYNC narrow/wide |
| Referências | Fluxo/VU, calibração e velocidades em funções isoladas | Integrar medição VU/calibrate ao plugin |
| Presets | 90 alinhamentos, programas do host, JSONs de fábrica | Diferenças de timbre dependem do DSP; import/export UI, usuário, busca/A-B |
| Estado | APVTS, schema v1, rejeição de estados incompletos/inválidos | Migrações de versões futuras |
| Interface | Painel com arte de referência, knobs, seletores, VUs RMS, contador, animações, abas e busca de presets | Validação em Studio One Pro 8 real |
| Tempo real | Ganhos suavizados, denormais, sem construção de strings ou alocação explícita no processBlock | Auditoria RT completa/automação sob carga |
| Transporte | Leitura do host, contador, zero persistido e animação | Varispeed, flutter, ruído e controle do host (não previsto) |

O mapeamento documental anterior permanece como especificação pretendida. Não interpretar uma entrada nesse documento como recurso já implementado. Os nomes de parâmetros do host preservam `[PENDING]`, inclusive tape_type/EQ. A interface visual mostra etiquetas limpas e explica as limitações em SETTINGS e tooltips. `[VISUAL]` marca controles sem efeito sonoro por requisito.

O número de programa indica o último preset carregado, mesmo após edição manual de parâmetros; o estado completo, não esse número, determina os ajustes restaurados.
