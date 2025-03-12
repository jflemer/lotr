#include "lotr.h"
#include "character.h"
#include "init.h"
#include "map.h"
#include "object.h"
#include "spot.h"
#include "game.h"
#include "utils.h"

// game.c
extern int game_maps[][4];

int
main(void)
{
    int i;
#ifndef TTT
    int max_map = 13;
#else
    int max_map = 15;
#endif
    FILE *chars;
    FILE *objs;
    FILE *quest;

    lotr_init();

    chars = fopen("characters.txt", "w");
    for (i = 0; i < CHARACTERS_NUM; ++i) {
        if (character_exists(i)) {
            const char *name = character_get(i)->name;
            if (name[0]) {
                fprintf(chars, "%s=\n", name);
            }
        }
    }

    objs = fopen("objects.txt", "w");
    for (i = 0; i < 0x100; ++i) {
        const char *name = object_name(i);
        if (name[0]) {
            fprintf(objs, "# %s\n%02x=\n\n", name, i);
        }
    }

    quest = fopen("questions.txt", "w");

#ifndef TTT
    i = 1;
#else
    i = 0;
#endif
    for (; i <= max_map; i += 2) {
        FILE *texts;
        FILE *spots;
        char name[64];
        int j;

        game_load_map(i);

        sprintf(name, "map%02d.txt", i / 2);
        texts = fopen(name, "w");
        for (j = 0; j < 0x100; ++j) {
            const char *text = game_get_text(j);
            if (strcmp(text, "<null>")) {
                fprintf(texts, "# %s\n%02x=\n\n", text, j);
            }
        }

        sprintf(name, "map%02d_spots.txt", i / 2);
        spots = fopen(name, "w");

        for (j = 0; j < map_get_num_spots(); ++j) {
            CommandSpot *spot = map_get_spot_number(j);
            int k;

            fputs(spot_get_string(spot), spots);
            fputs("\n\n", spots);

            for (k = 0; k < spot->commands_num; ++k) {
                Uint8 *data = spot->data + spot->command_start[k];
                if (data[0] == 0x36) {
                    /* NPC_SET_NAME */
                    fprintf(chars, "%s=\n", data + 2);
                }

                if (data[0] == 0x1c) {
                    /* NPC QUESTIONS */
                    /* ripped from spot_get_string */
                    int j = spot->command_start[k] + 2;
                    while (spot->data[j] != 0xff) {
                        int jj = 0;
                        char question_key[21];

                        while (spot_question_letter(spot->data[j]) && jj < 20) {
                            ++j;
                            ++jj;
                        }

                        strncpy(question_key, (char *)spot->data + j - jj, jj);
                        question_key[jj] = 0;

                        if (spot->data[j] == 0)
                            ++j;
                        else if (spot->data[j] == 0xff)
                            --j;
                        if (question_key[0])
                            fprintf(quest, "%s=\n", question_key);
                        ++j;
                    }
                }
            }
        }
        fclose(spots);

        fclose(texts);
    }

    fclose(chars);
    fclose(objs);
    fclose(quest);

    return 0;
}

