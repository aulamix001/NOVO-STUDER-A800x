# Download — A800 Technical Prototype 0.1.0

- [Baixar VST3 Windows x64](./A800-Technical-Prototype-Windows-x64.zip?raw=true)
- [Baixar código-fonte](./A800-Technical-Prototype-Source.zip?raw=true)
- [SHA256 dos pacotes](./SHA256SUMS.txt)

## Estado real desta versão

Este é um protótipo técnico, ainda não uma emulação do som analógico da A800. Possui ganhos/roteamento estéreo, estado e 90 programas de alinhamento. Saturação de fita, EQ e bias não estão implementados; os presets ainda não produzem 90 timbres distintos.

Os testes de áudio, estado e automação passaram no pluginval strictness 5 sem GUI. A validação completa teve timeout na interface sob Wine; funcionamento na DAW Windows real, incluindo Studio One Pro 8, ainda não foi confirmado. O pacote contém os logs e o relatório de validação. Este upload não corrige a falha de abertura.

## Instalação para teste no Windows x64

1. Baixe o ZIP usando o link acima ou o botão **Download raw file** do GitHub.
2. Extraia o arquivo.
3. Copie a pasta inteira `A800 Technical Prototype.vst3` para `C:\Program Files\Common Files\VST3\`.
4. Reinicie a DAW e faça nova busca de plugins VST3. O VST3 deve ser inserido numa faixa estéreo; não é um programa que abre com duplo clique.

Se não aparecer ou não abrir no Studio One Pro 8, informe a mensagem exata e se o problema ocorre na busca, ao inserir ou ao abrir o editor.
