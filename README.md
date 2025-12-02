# ⚔️ Fight Club: Elemental Combat

A 2-player local multiplayer fighting game featuring elemental spell combos, status effects, and real-time combat mechanics built with C++ and SFML.

![Game Version](https://img.shields.io/badge/version-1.0-blue)
![C++](https://img.shields.io/badge/C++-11-green)
![SFML](https://img.shields.io/badge/SFML-2.5+-red)

## 🎮 Game Overview

Fight Club: Elemental Combat is a competitive 2D fighting game where players battle using elemental magic. Combine Fire, Water, and Lightning elements to create powerful spell combos, activate shields, teleport, and unleash devastating ultimate attacks!

## ✨ Features

- **🔥 Elemental Combat System**: Fire, Water, and Lightning elements
- **🎯 Combo Mechanics**: Combine elements for special abilities
- **🛡️ Status Effects**: Burn, Stun, Slow, Paralyze
- **💫 Special Abilities**: Shield, Teleport, Ultimate Attack
- **🎨 Pixel Art Characters**: Custom player sprites with background removal
- **📊 Real-time HUD**: Health and Mana bars
- **🏆 Win Condition System**: Last player standing wins
- **🎵 Multiple Game States**: Menu, Controls, Playing, Game Over

## 🕹️ Controls

### Player 1 (Blue)
| Action | Key |
|--------|-----|
| Move Up | W |
| Move Down | S |
| Move Left | A |
| Move Right | D |
| Fire Element | 1 |
| Water Element | 2 |
| Lightning Element | 3 |
| Cast Spell | SPACE |

### Player 2 (Yellow)
| Action | Key |
|--------|-----|
| Move Up | ↑ |
| Move Down | ↓ |
| Move Left | ← |
| Move Right | → |
| Fire Element | I |
| Water Element | O |
| Lightning Element | P |
| Cast Spell | ENTER |

### Universal Controls
- **ESC**: Return to menu (during gameplay)

## 🪄 Spell Combos

| Combo | Elements | Mana Cost | Effect |
|-------|----------|-----------|--------|
| **Fire Ball** | Fire (1) | 10 MP | Basic damage + Burn (2s DoT) |
| **Water Bolt** | Water (2) | 10 MP | Basic damage |
| **Lightning Strike** | Lightning (3) | 10 MP | Basic damage + 10% Paralyze chance |
| **Shield** | Fire + Water | 20 MP | Block 10 HP damage for 5 seconds |
| **Teleport** | Fire + Lightning | 25 MP | Jump upward 150 pixels |
| **Slow Field** | Water + Lightning | 20 MP | Reduces target speed by 50% for 2s |
| **Ultimate** | Fire + Water + Lightning | 50 MP | 40 damage + 1.5s Stun |

### Combo System Rules
- Queue up to **3 elements** before casting
- Elements are sorted automatically (Fire → Water → Lightning)
- Incomplete combos won't cast (returns mana)
- Mana regenerates at **5 MP/second**

## 🎯 Status Effects

| Effect | Description | Duration |
|--------|-------------|----------|
| **Burn** 🔥 | Takes 2 damage per second | 2 seconds |
| **Stun** ⚡ | Cannot move or cast | 1-1.5 seconds |
| **Slow** ❄️ | Movement speed reduced by 50% | 2 seconds |
| **Paralyze** ⚡ | 10% chance to stun for 1 second | Instant |
| **Shield** 🛡️ | Absorbs 10 damage, blocks status effects | 5 seconds |

## 🏗️ Project Structure

```
FightClub/
├── main.cpp                 # Entry point
├── Game.h / Game.cpp        # Main game loop and state manager
├── Player.h / Player.cpp    # Player logic, movement, spells
├── Spell.h / Spell.cpp      # Projectile system
├── CollisionManager.h/cpp   # Hit detection
├── Platform.h / Platform.cpp # Environment obstacles
├── ShockManager.cpp         # Hardware feedback (Arduino integration)
├── arial.ttf                # Font file
├── player1.png              # Player 1 sprite
├── player2.png              # Player 2 sprite
├── background.jpg           # Background image
```

## 🎨 UML Class Diagram

The project follows object-oriented design principles:

- **Game**: Main controller managing game states and rendering
- **Player**: Handles player logic, HP/MP, spell casting
- **Spell**: Projectile system with damage and effects
- **CollisionManager**: Detects hits and environment collisions
- **Platform**: Static environment obstacles
- **ShockManager**: Hardware feedback integration
  <img width="1061" height="1079" alt="Uml_final" src="https://github.com/user-attachments/assets/03dd524a-deb5-4098-8fc3-b6b7e61dc85d" />

**Enjoy the battle! May the best elemental mage win!** ⚔️✨
