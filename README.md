# 🗡️ Dungeon Crawler: O Escolhido

Um jogo de RPG/Roguelite em turnos baseado em console (terminal), desenvolvido totalmente na linguagem C. Explore masmorras, resolva enigmas, aprimore seu equipamento e sobreviva para derrotar o temível lorde das trevas, Krauser.

## 📜 História

O sombrio Krauser aterroriza toda a vila, espalhando uma névoa de morte. Muitos tentaram invadir seu domínio, mas todos pereceram. As profecias sempre falaram de um **Escolhido** — alguém destinado a trazer o equilíbrio a este mundo caótico. Apenas aquele que transcender os limites de seu poder poderá empunhar as armas lendárias.

Lute. Sobreviva. E prove ser o guerreiro mais honrado de nossa era.

---

## ✨ Funcionalidades

* **Exploração em Turnos:** Navegue por 4 andares de masmorras geradas estaticamente com níveis crescentes de dificuldade.
* **Sistema de Combate Direcional:** O combate depende da direção que o personagem está olhando e da área de alcance (hitbox) da arma equipada.
* **Feedback Visual:** Sistema de renderização em sobreposição (overlay) que exibe as áreas de dano do ataque temporariamente (mostrando um `z` na área atingida), além de temas visuais com cores exclusivas para cada andar.
* **Armas e Evoluções (Ascensão):** Escolha sua classe no início e encontre o botão de ascensão para liberar o poder máximo da sua arma (desbloqueando a aura dourada do Escolhido).
* **IA de Inimigos Variada:** 
  * `X`: Movimenta-se e patrulha aleatoriamente.
  * `Y`: Persegue o jogador ativamente baseando-se na distância.
  * `Z` (Krauser - Boss): Teleporta-se pelo mapa e invoca lacaios `X` e `Y` constantemente para o campo de batalha.

---

## 🎮 Como Jogar

### Controles

* **W, A, S, D:** Movimentar o personagem e mudar a direção do olhar.
* **I:** Interagir com NPCs, portas trancadas, botões e escadarias.
* **O:** Atacar (a área de acerto e o dano dependem da sua arma).

### A Lenda do Mapa

| Símbolo | Descrição |
| :---: | --- |
| `^`, `v`, `<`, `>` | O Jogador (A ponta indica a direção do olhar) |
| `*` | Parede impenetrável |
| `k` | Caixa frágil (Destrua atacando para abrir caminho) |
| `#` | Espinhos letais (Cuidado, hit kill que reinicia o andar!) |
| `@` | Chave enferrujada (Usada para abrir portas) |
| `D` | Porta trancada (Requer uma chave para ser aberta) |
| `O` | Botão de Ascensão (Evolui o dano da arma de 1 para 3) |
| `L` | Escadaria para o próximo andar / área |
| `N` | Ancião da Vila (Interaja com `I` para escolher sua arma inicial) |

---

## ⚔️ O Arsenal do Escolhido

Ao falar com o Ancião no primeiro mapa, você deve escolher seu estilo de combate. Cada arma possui uma área de efeito (Hitbox) única, e os inimigos só tomam dano se estiverem dentro dela no momento do ataque:

| Escolha | Arma Inicial | Evolução (Ascensão) | Estilo de Ataque |
| :---: | --- | --- | --- |
| **1** | Wooden Sword | **Master Sword** | **Cleave Frontal:** Atinge uma grande área retangular de 2x3 blocos imediatamente à frente do jogador. |
| **2** | Patched Bow | **Galadhrim Bow** | **Longo Alcance:** Dispara uma flecha penetrante em uma linha reta de até 4 blocos de distância. |
| **3** | Hickory Staff | **Void Staff** | **Área Mágica (AoE):** Ataca todos os blocos adjacentes (área de 3x3 ao redor do jogador simultaneamente). |

---

## 🚀 Como Compilar e Executar

### Pré-requisitos

Este projeto foi construído utilizando bibliotecas nativas do ambiente Windows (`<windows.h>` e `<conio.h>`) para controle de interface, cores de terminal e captura de teclado em tempo real. Portanto, ele deve ser compilado e executado no sistema operacional **Windows**.

### Passo a Passo (Usando GCC/MinGW)

1. Clone o repositório em sua máquina:
```bash
git clone [https://github.com/Leonardo-G-Neves/dungeon-crawler-c.git](https://github.com/Leonardo-G-Neves/dungeon-crawler-c.git)
## 👨‍💻 Desenvolvedores e Créditos

Este projeto foi concebido e codificado com dedicação por:

* **Leonardo Augusto Gonçalves das Neves**
* **Adan Arthur Mathias de Castro Corrêa de Melo**
* **Arthur Moraes de Souza**

**Nota de Desenvolvimento:** Para garantir um código limpo em linguagem C clássica e refinar a estrutura de sistemas complexos — como o motor de renderização de mapa, feedback visual sobreposto e sistema de colisões —, ferramentas de Inteligência Artificial foram utilizadas para auxiliar pontualmente no desenvolvimento arquitetural do projeto.
