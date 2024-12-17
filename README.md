# Text-Based Adventure Game
A command-line-based adventure game in C that simulates a dungeon exploration scenario

This project is a text-based dungeon adventure game written in C. The player explores different rooms, interacts with creatures, collects items, and attempts to survive various challenges.

## Features

Dynamic player creation with attributes such as health, strength, and inventory.

Rooms with distinct descriptions, items, and potential creatures.

Fully connected room navigation (up, down, left, right).

Inventory system for picking up, using, and dropping items.

Combat mechanics to fight creatures.

Save and load functionality to manage game progress.

Special room effects, such as resting rooms and trap rooms.

## How to Play

### Commands

The game uses text commands for interaction. Below are the supported commands:

**look**: Displays the current room's description, items, and creatures.

**move <u/d/l/r>**: Moves the player in the specified direction (up, down, left, right).

**inventory**: Shows the player's current inventory.

**pickup**: Adds an item from the room to the player's inventory.

**use** : Uses an item from the inventory.

**drop** : Drops an item from the inventory into the current room.

**attack**: Attacks the creature in the current room (if any).

**list**: Lists all saved game files in the current directory.

**save** : Saves the current game state to a file.

**load** : Loads a previously saved game state.

**exit**: Exits the game.


## How to Compile
 Use a C compiler like gcc to compile the project:
 
``` gcc dungeon_game.c -o dungeon_game ```
## How to Run
Run the compiled executable:

``` ./dungeon_gam ```

## Code Structure

### Key Components

#### 1. Player Structure:

- health: Player's current health points.

- strength: Player's attack strength.

- inventory: Dynamic array of strings representing items.

- inventory_size: Current number of items in the inventory.

- inventory_capacity: Maximum number of items the player can carry.

#### 2. Room Structure:

- description: Dynamic string describing the room.

- items: Dynamic array of strings representing items in the room.

- item_count: Number of items in the room.

- creature: Pointer to a creature in the room (if any).

- up, down, left, right: Pointers to neighboring rooms.

#### 3. Creature Structure:

- name: Dynamic string for the creature's name.

- health: Creature's current health points.

- strength: Creature's attack strength.

### Functions

- create_player: Initializes a new player with default attributes.

- create_room: Allocates and initializes a new room.

- free_player / free_room: Frees allocated memory for players and rooms.

- look: Displays details about the current room.

- move: Moves the player between connected rooms.

- attack_creature: Handles combat between the player and a creature.

- pickup_item / drop_item: Manages item transfer between the player and the room.

- use_item: Applies the effects of an item from the inventory.

- save_game / load_game: Saves and loads the game state to/from a file.

- list_saved_games: Lists all saved game files in the current directory.

### Special Rooms

#### Resting Room:

* Description: "Burasi guvenli bir dinlenme odasi."

* Effect: Restores 10 health points (up to a maximum of 100).

#### Trap Room:

* Description: "Burasi tehlikeli tuzaklarla dolu oda."

* Effect: Automatically triggers combat with the creature in the room.

### Dependencies

Standard C libraries: *stdio.h*, *stdlib.h*, *string.h*

Additional library for file operations: *dirent.h*

### Memory Management

All dynamically allocated memory for players, rooms, creatures, and items is freed before the program exits. Make sure to follow proper memory management to avoid leaks.
