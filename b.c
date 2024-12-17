#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Oyuncuyu temsil eden yapi
typedef struct
{
    int health;
    int strength;
    char **inventory;       // Dinamik string dizisi
    int inventory_size;     // su anki envanterdeki esya sayisi
    int inventory_capacity; // Tasinabilecek maksimum esya sayisi
} Player;

// Yaratigi temsil eden yapi
typedef struct
{
    char *name; // Dinamik string
    int health;
    int strength;
} Creature;

// Odayi temsil eden yapi
typedef struct Room
{
    char *description;  // Dinamik string
    char **items;       // Odadaki esyalar
    int item_count;     // Kac esya var
    Creature *creature; // Odadaki yaratik (varsa)
    struct Room *up;    // Yukaridaki oda
    struct Room *down;  // Asagidaki oda
    struct Room *left;  // Soldaki oda
    struct Room *right; // Sagdaki oda
} Room;

// Oyuncu olusturma
Player *create_player()
{
    Player *player = (Player *)malloc(sizeof(Player));
    if (!player)
    {
        printf("Bellek tahsis hatasi!\n");
        exit(1);
    }
    player->health = 100;     // Varsayilan can
    player->strength = 10;    // Varsayilan saldiri gucu
    player->inventory = NULL; // Envanter baslangicta bos
    player->inventory_size = 0;
    player->inventory_capacity = 3; // ornegin, oyuncunun tasiyabilecegi maksimum esya sayisi
    return player;
}

// Oda olusturma
Room *create_room(const char *description)
{
    Room *room = (Room *)malloc(sizeof(Room));
    if (!room)
    {
        printf("Bellek tahsis hatasi!\n");
        exit(1);
    }
    room->description = strdup(description); // String kopyalama
    room->items = NULL;                      // Baslangicta esya yok
    room->item_count = 0;
    room->creature = NULL;                                   // Yaratik yok
    room->up = room->down = room->left = room->right = NULL; // Tum baglantilar bos
    return room;
}

// Bellegi serbest birakma
void free_room(Room *room)
{
    if (room)
    {
        free(room->description); // Oda aciklamasini serbest birak
        if (room->items)
        {
            for (int i = 0; i < room->item_count; i++)
            {
                free(room->items[i]); // Odadaki her bir esya
            }
            free(room->items);
        }
        if (room->creature)
        {
            free(room->creature->name); // Yaratik adi
            free(room->creature);       // Yaratik
        }
        free(room); // Odanin kendisi
    }
}

void free_player(Player *player)
{
    if (!player)
        return;
    for (int i = 0; i < player->inventory_size; i++)
    {
        free(player->inventory[i]);
    }
    free(player->inventory);
    free(player);
}

void look(Room *current_room, Player *player)
{
    if (!current_room)
        return;
    printf("Oda: %s\n", current_room->description);
    if (current_room->item_count > 0)
    {
        printf("Esyalar:\n");
        for (int i = 0; i < current_room->item_count; i++)
        {
            printf(" - %s\n", current_room->items[i]);
        }
    }
    if (current_room->creature)
    {
        printf("Bu odada bir yaratik var: %s (Can: %d)\n",
               current_room->creature->name, current_room->creature->health);
    }
    // Dinlenme odasinda can artisi
    if (strcmp(current_room->description, "Burasi guvenli bir dinlenme odasi.") == 0)
    {
        int heal_amount = 10;
        if (player->health < 100)
        {
            player->health += heal_amount;
            if (player->health > 100)
                player->health = 100; // Can maksimum 100 olabilir
            printf("Dinlenme odasinda caniniz %d artti. su anki caniniz: %d\n", heal_amount, player->health);
        }
        else
        {
            printf("Caniniz zaten tam durumda.\n");
        }
    }
}

void attack_creature(Player *player, Room *current_room)
{
    if (!current_room->creature)
    {
        printf("Bu odada saldirabileceginiz bir yaratik yok.\n");
        return;
    }

    Creature *creature = current_room->creature;

    printf("'%s' ile savasiyorsunuz!\n", creature->name);

    while (player->health > 0 && creature->health > 0)
    {
        // Oyuncunun saldirisi
        creature->health -= player->strength;
        printf("Yaratiga saldirdiniz. Yaratigin kalan cani: %d\n", creature->health);

        if (creature->health <= 0)
        {
            printf("Yaratigi oldurdunuz!\n");
            free(creature->name);
            free(creature);
            current_room->creature = NULL;
            return;
        }

        // Yaratigin saldirisi
        player->health -= creature->strength;
        printf("Yaratik size saldirdi. Kalan caniniz: %d\n", player->health);

        if (player->health <= 0)
        {
            printf("Yaratik sizi oldurdu. Oyun bitti!\n");
            exit(0);
        }
    }
}

void use_item(Player *player, const char *item_name)
{
    if (strcmp(item_name, "HealthPotion") == 0)
    {
        if (player->health < 100)
        {
            player->health += 20;
            if (player->health > 100)
                player->health = 100;
            printf("Sağlik iksiri kullandiniz. Caniniz artti: %d\n", player->health);
        }
        else
        {
            printf("Sağliğiniz zaten tam durumda!\n");
        }
    }
    else if (strcmp(item_name, "PowerPotion") == 0)
    {
        player->strength += 5;
        printf("Guc iksiri kullandiniz. Saldiri gucunuz artti: %d\n", player->strength);
    }

    else
    {
        printf("Bu esya kullanilabilir değil.\n");
    }
}

Room *move(Room *current_room, char direction, Player *player)
{
    Room *next_room = NULL;
    switch (direction)
    {
    case 'u':
        next_room = current_room->up;
        break;
    case 'd':
        next_room = current_room->down;
        break;
    case 'l':
        next_room = current_room->left;
        break;
    case 'r':
        next_room = current_room->right;
        break;
    default:
        printf("Gecersiz yon!\n");
        break;
    }
    if (next_room)
    {
        printf("Yeni bir odaya girdiniz: %s\n", next_room->description);
        // Tuzak odasinda otomatik saldiri
        if (strcmp(next_room->description, "Burasi tehlikeli tuzaklarla dolu oda.") == 0 && next_room->creature)
        {
            printf("Tuzak odasina girdiniz! %s size saldiriyor!\n", next_room->creature->name);
            attack_creature(player, next_room);
        }
    }
    else
    {
        printf("Bu yonde bir oda yok!\n");
        next_room = current_room;
    }
    return next_room;
}

void show_inventory(Player *player)
{
    printf("Envanteriniz:\n");
    if (player->inventory_size == 0)
    {
        printf("Envanteriniz bos.\n");
        return;
    }
    for (int i = 0; i < player->inventory_size; i++)
    {
        printf(" - %s\n", player->inventory[i]);
    }
}

void pickup_item(Player *player, Room *current_room, const char *item_name)
{
    if (player->inventory_size >= player->inventory_capacity)
    { // Maksimum kapasiteyi kontrol et
        printf("Envanter dolu! Daha fazla esya tasiyamazsiniz.\n");
        return;
    }
    for (int i = 0; i < current_room->item_count; i++)
    {
        if (strcmp(current_room->items[i], item_name) == 0)
        {
            player->inventory = realloc(player->inventory, (player->inventory_size + 1) * sizeof(char *));
            player->inventory[player->inventory_size++] = strdup(item_name);
            printf("%s envanterinize eklendi.\n", item_name);
            free(current_room->items[i]);
            for (int j = i; j < current_room->item_count - 1; j++)
            {
                current_room->items[j] = current_room->items[j + 1];
            }
            current_room->item_count--;
            return;
        }
    }
    printf("Bu esya odada yok!\n");
}
void drop_item(Player *player, Room *current_room, const char *item_name)
{
    for (int i = 0; i < player->inventory_size; i++)
    {
        if (strcmp(player->inventory[i], item_name) == 0)
        {
            // Odaya esya ekle
            current_room->items = realloc(current_room->items, (current_room->item_count + 1) * sizeof(char *));
            current_room->items[current_room->item_count++] = strdup(item_name);

            // Envanterden esya cikar
            free(player->inventory[i]);
            for (int j = i; j < player->inventory_size - 1; j++)
            {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory_size--;

            printf("%s odaya birakildi.\n", item_name);
            return;
        }
    }
    printf("Bu esya envanterinizde yok!\n");
}

// Oyun dosyalarini yonetmek icin eklenen fonksiyonlar...
#include <dirent.h> // Dosya/dizin islemleri icin

void list_saved_games()
{
    struct dirent *entry;
    DIR *dir = opendir("."); // Mevcut dizini acar
    if (!dir)
    {
        printf("Kayitli oyunlari listelemek icin dizin acilamadi.\n");
        return;
    }

    printf("Kayitli oyunlar:\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".sav")) // ".sav" uzantili dosyalari filtreler
        {
            printf(" - %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void save_game(const char *filepath, Player *player, Room *current_room)
{
    FILE *file = fopen(filepath, "w");
    if (!file)
    {
        printf("Oyun durumu kaydedilemedi: %s\n", filepath);
        return;
    }

    // Oyuncu bilgilerini yaz
    fprintf(file, "Player\n");
    fprintf(file, "Health:%d\n", player->health);
    fprintf(file, "Strength:%d\n", player->strength);
    fprintf(file, "InventorySize:%d\n", player->inventory_size);
    fprintf(file, "InventoryCapacity:%d\n", player->inventory_capacity); // Eklenen kapasiteyi de yaziyoruz
    for (int i = 0; i < player->inventory_size; i++)
    {
        fprintf(file, "Item:%s\n", player->inventory[i]);
    }

    // Mevcut oda bilgilerini yaz
    fprintf(file, "CurrentRoom\n");
    fprintf(file, "Description:%s\n", current_room->description);

    fclose(file);
    printf("Oyun durumu '%s' dosyasina kaydedildi.\n", filepath);
}
Room *load_game(const char *filepath, Player *player, Room *rooms[])
{
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        printf("Oyun durumu yuklenemedi: %s\n", filepath);
        return NULL;
    }

    char line[256];
    Room *current_room = NULL;

    while (fgets(line, sizeof(line), file))
    {
        // Oyuncu bilgilerini oku
        if (strncmp(line, "Player", 6) == 0)
        {
            fgets(line, sizeof(line), file); // Saglik
            sscanf(line, "Health:%d", &player->health);

            fgets(line, sizeof(line), file); // Guc
            sscanf(line, "Strength:%d", &player->strength);

            fgets(line, sizeof(line), file); // Envanter boyutu
            sscanf(line, "InventorySize:%d", &player->inventory_size);

            player->inventory = (char **)malloc(sizeof(char *) * player->inventory_size);
            for (int i = 0; i < player->inventory_size; i++)
            {
                fgets(line, sizeof(line), file); // Esyalar
                char item_name[50];
                sscanf(line, "Item:%s", item_name);
                player->inventory[i] = strdup(item_name);
            }
        }
        else if (strncmp(line, "CurrentRoom", 11) == 0)
        {
            fgets(line, sizeof(line), file); // Oda aciklamasi
            char description[256];
            sscanf(line, "Description:%[^\n]", description);

            // Mevcut oda olusturuluyor
            current_room = create_room(description);
        }
    }

    fclose(file);
    printf("Oyun durumu '%s' dosyasindan yuklendi.\n", filepath);
    return current_room;
}
int main()
{
    // Oyuncu ve odalar olusturuluyor
    Player *player = create_player();

    Room *room0 = create_room("Burasi giris koridoru");
    Room *shop = create_room("Burasi bir esya odasi.");
    Room *mystery_room = create_room("Burada gizemli esyalar var!");
    Room *monster_room = create_room("Burasi bir yaratik odasi!");
    Room *recreation_room = create_room("Burasi guvenli bir dinlenme odasi.");
    Room *trap_room = create_room("Burasi tehlikeli tuzaklarla dolu oda.");

    // Odalar birbirine baglaniyor
    room0->up = shop;
    shop->down = room0;
    room0->left = monster_room;
    monster_room->right = room0;
    room0->right = mystery_room;
    mystery_room->left = room0;
    room0->down = recreation_room;
    recreation_room->up = room0;
    trap_room->up = mystery_room;
    mystery_room->down = trap_room;
    trap_room->left = recreation_room;
    recreation_room->right = trap_room;

    // Esyalar ekleniyor 1. oda
    shop->items = (char **)malloc(5 * sizeof(char *));
    shop->items[0] = strdup("Sword");
    shop->items[1] = strdup("Axe");
    shop->items[2] = strdup("Arrow");
    shop->items[3] = strdup("Spear");
    shop->items[4] = strdup("Bomb");
    shop->item_count = 5;
    // Esyalar ekleniyor 2. oda
    mystery_room->items = (char **)malloc(4 * sizeof(char *));
    mystery_room->items[0] = strdup("DustyBook");
    mystery_room->items[1] = strdup("Map");
    mystery_room->items[2] = strdup("MagicRing");
    mystery_room->items[3] = strdup("FireballStaff");
    mystery_room->item_count = 4;

    // Esyalar ekleniyor 3. oda (recreation_room)
    recreation_room->items = (char **)malloc(2 * sizeof(char *));
    recreation_room->items[0] = strdup("HealthPotion");
    recreation_room->items[1] = strdup("PowerPotion");
    recreation_room->item_count = 2;

    // Bir yaratik ekleniyor (trap_room)
    Creature *trap_creature = (Creature *)malloc(sizeof(Creature));
    trap_creature->name = strdup("olumcul Tuzak Canavari");
    trap_creature->health = 70;
    trap_creature->strength = 5;
    trap_room->creature = trap_creature;
    // Bir yaratik ekleniyor
    Creature *creature = (Creature *)malloc(sizeof(Creature));
    creature->name = strdup("Korkunc bir kurt");
    creature->health = 50;
    creature->strength = 15;
    monster_room->creature = creature;

    // Oyuncu baslangic odasinda
    Room *current_room = room0;

    // Oyun dongusu
    char command[20];
    while (1)
    {
        printf("\nKomutlar: look, move <u/d/l/r>, inventory, pickup <item>, use <item>, drop <item>, attack, list, save <filepath>, load <filepath>, exit\n");
        printf("Komut girin: ");
        scanf("%s", command);

        if (strcmp(command, "look") == 0)
        {
            look(current_room, player);
        }
        else if (strcmp(command, "move") == 0)
        {
            char direction;
            scanf(" %c", &direction);
            current_room = move(current_room, direction, player);
        }
        else if (strcmp(command, "inventory") == 0)
        {
            show_inventory(player);
        }
        else if (strcmp(command, "pickup") == 0)
        {
            char item_name[50];
            scanf("%s", item_name);
            pickup_item(player, current_room, item_name);
        }
        else if (strcmp(command, "drop") == 0)
        {
            char item_name[50];
            scanf("%s", item_name);
            drop_item(player, current_room, item_name);
        }
        else if (strcmp(command, "use") == 0)
        {
            char item_name[50];
            scanf("%s", item_name);
            use_item(player, item_name);
        }
        else if (strcmp(command, "attack") == 0)
        {
            attack_creature(player, current_room);
        }
        else if (strcmp(command, "list") == 0)
        {
            list_saved_games();
        }
        else if (strcmp(command, "save") == 0)
        {
            char filepath[50];
            scanf("%s", filepath);
            save_game(filepath, player, current_room);
        }
        else if (strcmp(command, "load") == 0)
        {
            char filepath[50];
            scanf("%s", filepath);
            current_room = load_game(filepath, player, NULL);
        }
        else if (strcmp(command, "exit") == 0)
        {
            printf("Oyundan cikiliyor...\n");
            break;
        }
        else
        {
            printf("Bilinmeyen bir komut girdiniz.\n");
        }
    }

    // Bellegi temizleme
    free_room(room0);
    free_room(shop);
    free_room(mystery_room);
    free_room(recreation_room);
    free_room(monster_room);
    free_room(trap_room);
    free_player(player);

    return 0;
}
// bilgenur