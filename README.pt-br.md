# Docked

<p align="center">
  <strong>Um gerenciador integrado de projetos, compilação e dependências para C e C++</strong>
</p>

<p align="center">
  <a href="README.pt-br.md">Versão em Português</a> •
  <a href="README.md">English Version</a>
</p>

---

## Visão Geral

O desenvolvimento de aplicações em **C** e **C++** envolve frequentemente a utilização coordenada de diferentes ferramentas para compilação, construção e gerenciamento de dependências, tais como GCC, Clang, MSVC, CMake, Make, Ninja, vcpkg e Conan. Embora cada ferramenta atenda a necessidades específicas, a necessidade de combiná-las manualmente aumenta a complexidade do ambiente e impõe barreiras de entrada para novos desenvolvedores.

Inspirado em ferramentas integradas de outros ecossistemas modernos — como o **Cargo** na linguagem Rust —, o **Docked** centraliza essas responsabilidades em uma interface unificada. A partir de um único arquivo de manifesto (`docked.toml`) e comandos simples na linha de comando, o Docked gerencia a criação da estrutura de projetos, resolução de dependências, geração de scripts de construção de alta performance e compilação do executável final.

---

## Recursos

- **Fluxo de Trabalho Unificado**: Comandos intuitivos para todo o ciclo de vida: `docked init`, `docked build` e `docked run`.
- **Construção Rápida com Ninja**: Gera arquivos de build otimizados para o [Ninja](https://ninja-build.org/) com rastreamento incremental de dependências de cabeçalhos (`-MMD -MF`), evitando recompilações desnecessárias.
- **Suporte Nativo a C e C++**:
  - **C**: Suporte a C89, C90, C99, C11, C17 e C23.
  - **C++**: Suporte a C++11, C++14, C++17, C++20, C++23 e C++26.
- **Suporte a Múltiplos Compiladores com Detecção Automática**:
  - Compatível com **GCC**, **Clang** e **MSVC** (`cl`).
  - Identifica automaticamente o compilador padrão disponível no sistema operacional (Linux, macOS, Windows).
  - Mapeamento inteligente para o padrão suportado mais próximo em compiladores com restrições (ex.: MSVC).
- **Gerenciamento de Dependências Baseado em Git**:
  - Declaração simples de dependências via URL do repositório Git e tag/versão.
  - Clonagem e checkout automáticos no diretório local `.docked/dependencies/`.
- **Convenção sobre Configuração**: Estrutura de pastas padronizada e limpa (`src/` para código-fonte e `build/` para artefatos gerados).

---

## Pré-requisitos

Para compilar e executar o Docked a partir do código-fonte, são necessários:

- **Compilador com suporte a C++17** (ex.: GCC >= 9, Clang >= 10 ou MSVC 2019+)
- **CMake** (versão 3.15 ou superior)
- **Boost** (módulo `boost_program_options`)
- **Ninja** (`ninja` ou `ninja-build`)
- **Git** (utilizado para baixar dependências de projetos)

### Instalação das dependências

No Ubuntu / Debian:
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build libboost-program-options-dev git
```

No Fedora / RHEL:
```bash
sudo dnf install gcc-c++ cmake ninja-build boost-devel git
```

No macOS (via Homebrew):
```bash
brew install cmake ninja boost git
```

---

## Compilação e Instalação

Clone o repositório e compile o projeto utilizando o CMake:

```bash
# Clonar o repositório
git clone https://github.com/pedroafonso/Docked.git
cd Docked

# Configurar e compilar
cmake -B build -S .
cmake --build build

# Executar a suíte de testes
ctest --test-dir build --output-on-failure
```

O binário executável será gerado em `build/docked`. Você pode colocá-lo no `PATH` do sistema (por exemplo, em `/usr/local/bin/`).

---

## Guia Rápido de Uso

### 1. Inicializar um Novo Projeto

Crie um novo diretório e inicialize o projeto com o Docked:

```bash
# Inicializar no diretório atual
docked init

# Ou criar um projeto nomeado em uma subpasta
docked init meu-app --name "meu-app"
cd meu-app
```

Isso criará a seguinte estrutura:

```
meu-app/
├── docked.toml
└── src/
    └── main.cpp (ou main.c)
```

### 2. Configurar o `docked.toml`

Edite o arquivo `docked.toml` conforme as necessidades do seu projeto:

#### Exemplo de Projeto em C++
```toml
[project]
name = "MeuAppCpp"
version = 1.0
language = "C++"       # Opções: "C++" ou "C"
compiler = "default"   # Opções: "default", "G++", "CLANG" ou "MSVC"
standard = 20          # 11, 14, 17, 20, 23 ou 26

[dependencies]
# Exemplo de dependência via Git
# name = "googletest"
# git_url = "https://github.com/google/googletest.git"
# git_tag = "v1.14.0"
```

#### Exemplo de Projeto em C
```toml
[project]
name = "MeuAppC"
version = 1.0
language = "C"         # Compilação utilizando toolchain C
compiler = "default"   # Opções: "default", "GCC", "CLANG" ou "MSVC"
standard = 17          # 89, 90, 99, 11, 17 ou 23

[dependencies]
```

### 3. Compilar o Projeto

```bash
docked build
```

O Docked executará as seguintes etapas:
1. Resolução e clonagem das dependências listadas em `docked.toml`.
2. Varredura recursiva de arquivos-fonte dentro da pasta `src/`.
3. Geração do arquivo `build/build.ninja` com as regras e flags adequadas.
4. Execução do `ninja -C build`, gerando o binário final na pasta `build/`.

### 4. Executar o Programa

```bash
docked run
```

O comando executará o binário correspondente recém-compilado na pasta `build/`.

---

## Referência de Comandos da CLI

| Comando | Argumentos | Descrição |
|---|---|---|
| `docked init [caminho]` | `[caminho]` (padrão: `.`)<br>`--name <nome>` | Cria a estrutura básica do projeto com `docked.toml` e `src/main.cpp`. |
| `docked build` | *Nenhum* | Resolve dependências, gera o `build.ninja` e compila o projeto. |
| `docked run` | *Nenhum* | Executa o executável compilado localizado em `build/`. |
| `docked --help` / `-h` | *Nenhum* | Exibe as instruções de uso e ajuda da CLI. |

---

## Referência de Configuração (`docked.toml`)

### Seção `[project]`

| Campo | Tipo | Padrão | Descrição |
|---|---|---|---|
| `name` | texto | `"project"` | Nome do projeto e do executável final gerado. |
| `version` | número | `1.0` | Versão do projeto. |
| `language` | texto | `"C++"` | Linguagem principal do projeto: `"C"` ou `"C++"`. |
| `compiler` | texto | `"default"` | Compilador: `"default"`, `"GCC"` / `"G++"`, `"CLANG"` ou `"MSVC"`. |
| `standard` | inteiro | `26` (C++) / `23` (C) | Padrão da linguagem a ser utilizado (ver tabela abaixo). |

#### Padrões e Flags de Linguagem

| Linguagem | Valores de `standard` | Flag no GCC / Clang | Flag no MSVC (`cl`) |
|---|---|---|---|
| **C** | `89`, `90` | `-std=c89`, `-std=c90` | `/std:c11` *(padrão suportado mais próximo)* |
| **C** | `99` | `-std=c99` | `/std:c11` *(padrão suportado mais próximo)* |
| **C** | `11` | `-std=c11` | `/std:c11` |
| **C** | `17` | `-std=c17` | `/std:c17` |
| **C** | `23` | `-std=c23` | `/std:clatest` |
| **C++** | `11` | `-std=c++11` | `/std:c++14` *(padrão suportado mais próximo)* |
| **C++** | `14` | `-std=c++14` | `/std:c++14` |
| **C++** | `17` | `-std=c++17` | `/std:c++17` |
| **C++** | `20` | `-std=c++20` | `/std:c++20` |
| **C++** | `23` | `-std=c++23` | `/std:c++latest` |
| **C++** | `26` (padrão) | `-std=c++26` | `/std:c++latest` |

---

### Seção `[dependencies]`

Para declarar bibliotecas externas via repositórios Git:

```toml
[dependencies]
name = "cjson"
git_url = "https://github.com/DaveGamble/cJSON.git"
git_tag = "v1.7.18"
```

Para múltiplos pacotes, podem ser usadas tabelas nomeadas:

```toml
[dependencies.cjson]
name = "cjson"
git_url = "https://github.com/DaveGamble/cJSON.git"
git_tag = "v1.7.18"

[dependencies.raylib]
name = "raylib"
git_url = "https://github.com/raysan5/raylib.git"
git_tag = "5.0"
```

As dependências são clonadas em `.docked/dependencies/<nome>` e alternadas para a `git_tag` configurada.

---

## Arquitetura e Funcionamento Interno

```
   ┌────────────────┐
   │  docked.toml   │
   └───────┬────────┘
           │ Leitura e parsing de configurações
           ▼
   ┌────────────────┐
   │  Dependências  │ ──► Git clone & checkout em .docked/dependencies/
   └───────┬────────┘
           │ Busca de arquivos-fonte em src/
           ▼
   ┌────────────────┐
   │ Gerador Ninja  │ ──► Gera build/build.ninja (regras, flags, objetos)
   └───────┬────────┘
           │ Execução do comando ninja -C build
           ▼
   ┌────────────────┐
   │ Executável     │ ──► Gerado em build/<nome_do_projeto>
   └────────────────┘
```

1. **Interpretação da Configuração**: O arquivo `docked.toml` é lido utilizando a biblioteca `toml++`.
2. **Resolução de Dependências**: O módulo de dependências obtém e configura os repositórios remotos necessários em `.docked/dependencies/`.
3. **Resolução do Compilador e Padrão**: As flags apropriadas de acordo com o sistema operacional, compilador e padrão de linguagem (C ou C++) são determinadas.
4. **Geração do Arquivo Ninja**: O Docked mapeia recursivamente os arquivos (`.c` ou `.cpp`) em `src/`, adiciona regras de pré-processamento e dependência incremental (`-MMD -MF`) e gera `build/build.ninja`.
5. **Construção e Execução**: O Ninja executa a compilação paralela e o linker cria o executável final.

---

## Contexto Acadêmico

O **Docked** foi concebido e desenvolvido por **Pedro Afonso Silva da Costa** sob orientação do **Prof. Me. Daniel Ricardo de Souza**, no âmbito do curso de **Análise e Desenvolvimento de Sistemas** da **Universidade Luterana do Brasil (Ulbra)** – Campus Torres/RS.

---

## Licença

Este projeto é de código aberto. Consulte o repositório para mais detalhes sobre os termos de licenciamento.
