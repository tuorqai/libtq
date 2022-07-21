
//------------------------------------------------------------------------------
// tq library example: jestan.c
//------------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <tq/tq.h>

//------------------------------------------------------------------------------

#define CAMERA_WIDTH            (256)
#define CAMERA_HEIGHT           (240)

#define MAX_PLAYERS             4

#define PLAYER_BIT_ACTIVE       (1 << 0)
#define PLAYER_BIT_VISIBLE      (1 << 1)

#define DIRECTION_UP            0
#define DIRECTION_DOWN          1
#define DIRECTION_LEFT          2
#define DIRECTION_RIGHT         3

#define MAX_OBJECTS             64

#define OBJECT_BIT_ACTIVE       (1 << 0)
#define OBJECT_BIT_VISIBLE      (1 << 1)

#define STAGE_WIDTH             20
#define STAGE_HEIGHT            32
#define STAGE_VOLUME            (STAGE_WIDTH * STAGE_HEIGHT)

//------------------------------------------------------------------------------

uint8_t stage_data[STAGE_VOLUME] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  44, 0,  0,  0,  0,  0,  46, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  44, 0,  0,  0,  0,  0,  46, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  44, 0,  0,  0,  0,  0,  46, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  44, 0,  0,  0,  0,  0,  46, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  44, 0,  0,  0,  0,  0,  46, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  64, 65, 65, 48, 0,  0,  46, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  70, 70, 70, 64, 65, 65, 66, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  70, 70, 70, 70, 0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  47, 65, 65, 65, 90, 65, 65, 65,
    65, 65, 65, 65, 90, 65, 65, 65, 48, 0,  0,  0,  46, 70, 70, 70, 88, 70, 70, 70,
    69, 69, 69, 69, 88, 69, 69, 69, 44, 0,  0,  0,  46, 0,  0,  0,  0,  0,  0,  0, 
    70, 70, 70, 70, 88, 70, 70, 70, 64, 65, 65, 65, 66, 0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  70, 70, 70, 70, 70, 0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  80, 81, 82, 0,  0,  0, 
    0,  0,  0,  0,  80, 83, 83, 83, 83, 83, 82, 0,  80, 81,101,101,102, 0,  0,  0, 
    0,  0,  0,  0,  84, 0,  0,  0,  0,  0, 100, 81,101,101,101,101,102, 0,  0,  0, 
    0,  0,  0,  0,  84, 0,  0,  0,  0,  0, 100,101,101,101,101,101,102, 0,  0,  0, 
    0,  0,  0,  0,  84, 0,  0,  0,  0,  0, 120,121,121,121,121,121,122, 0,  0,  0, 
    0,  0,  0,  0,  84, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
};

typedef struct stage
{
    uint8_t     tiles[STAGE_VOLUME];
    tq_texture  texture;
} stage_t;

typedef struct camera
{
    float       x;
    float       y;
    float       rotation;
} camera_t;

typedef struct players
{
    uint32_t    bits[MAX_PLAYERS];
    float       x[MAX_PLAYERS];
    float       y[MAX_PLAYERS];
    float       dx[MAX_PLAYERS];
    float       dy[MAX_PLAYERS];
    int         direction[MAX_PLAYERS];
    float       animation_tick[MAX_PLAYERS];
    int         frame[MAX_PLAYERS];

    tq_texture  texture;
    int32_t     current;
} players_t;

typedef struct objects
{
    uint32_t    bits[MAX_OBJECTS];
    float       x[MAX_OBJECTS];
    float       y[MAX_OBJECTS];
    
    float       spawn_tick;
    tq_texture  texture;
    int32_t     current;
} objects_t;

typedef struct world
{
    float       dt;
    float       ct;

    stage_t     stage;
    camera_t    camera;
    players_t   players;
    objects_t   objects;

    int32_t     player_id;

    tq_font     font;
} world_t;

//------------------------------------------------------------------------------

void spawn_stage(stage_t *self)
{
    memcpy(self->tiles, stage_data, STAGE_VOLUME);

    for (int q = 0; q < STAGE_HEIGHT; q++) {
        for (int p = 0; p < STAGE_WIDTH; p++) {
            if (self->tiles[q * STAGE_WIDTH + p] == 0) {
                float r = rand() / (float) RAND_MAX;

                if (r < 0.06f) {
                    self->tiles[q * STAGE_WIDTH + p] = 7 + rand() % 2;
                } else if (r < 0.15f) {
                    self->tiles[q * STAGE_WIDTH + p] = 27 + rand() % 2;
                } else if (r < 0.33f) {
                    self->tiles[q * STAGE_WIDTH + p] = 129 + rand() % 2;
                }
            }
        }
    }
}

void update_stage(world_t *world, stage_t *self)
{
}

void draw_stage(stage_t const *self)
{
    tq_fill_rectangle(TQ_RECTF(0.0f, 0.0f, STAGE_WIDTH * 16.0f, STAGE_HEIGHT * 16.0f));

    for (int q = 0; q < STAGE_HEIGHT; q++) {
        for (int p = 0; p < STAGE_WIDTH; p++) {
            int c = self->tiles[q * STAGE_WIDTH + p];

            if (c == 0) {
                continue;
            }

            int x = c % 20;
            int y = c / 20;

            tq_push_matrix();
            tq_translate_matrix(TQ_VEC2F(p * 16.0f, q * 16.0f));
            tq_draw_subtexture(
                self->texture,
                TQ_RECTF(x * 16.0f, y * 16.0f, 16.0f, 16.0f),
                TQ_RECTF(0.0f, 0.0f, 16.0f, 16.0f)
            );
            tq_pop_matrix();
        }
    }
}

bool is_tile_solid(stage_t const *self, int p, int q)
{
    if (p < 0 || p >= STAGE_WIDTH || q < 0 || q >= STAGE_HEIGHT) {
        return true;
    }

    int c = self->tiles[q * STAGE_WIDTH + p];

    switch (c) {
    case 4:
    case 24:
    case 25:
    case 26:
    case 44:
    case 45:
    case 46:
    case 64:
    case 65:
    case 66:
    case 69:
    case 70:
    case 71:
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

void update_camera(world_t *world, camera_t *self)
{
    self->x = world->players.x[world->player_id] + 16.0f;
    self->y = world->players.y[world->player_id] + 16.0f;

    if (self->x < CAMERA_WIDTH / 2.0f) {
        self->x = CAMERA_WIDTH / 2.0f;
    }

    if (self->x > (STAGE_WIDTH * 16.0f) - CAMERA_WIDTH / 2.0f) {
        self->x = (STAGE_WIDTH * 16.0f) - CAMERA_WIDTH / 2.0f;
    }

    if (self->y < CAMERA_HEIGHT / 2.0f) {
        self->y = CAMERA_HEIGHT / 2.0f;
    }

    if (self->y > (STAGE_HEIGHT * 16.0f) - CAMERA_HEIGHT / 2.0f) {
        self->y = (STAGE_HEIGHT * 16.0f) - CAMERA_HEIGHT / 2.0f;
    }

    if (tq_is_key_pressed(TQ_KEY_COMMA)) {
        self->rotation -= 36.0f * world->dt;
    }

    if (tq_is_key_pressed(TQ_KEY_PERIOD)) {
        self->rotation += 36.0f * world->dt;
    }

    if (tq_is_key_pressed(TQ_KEY_SPACE)) {
        self->rotation = 0.0f;
    }
}

//------------------------------------------------------------------------------

int32_t spawn_player(players_t *self)
{
    int32_t id = self->current;
    self->current = (self->current + 1) % MAX_PLAYERS;

    self->bits[id] = PLAYER_BIT_ACTIVE | PLAYER_BIT_VISIBLE;
    self->x[id] = 0.0f;
    self->y[id] = 0.0f;

    return id;
}

void update_players(world_t *world, players_t *self)
{
    for (int32_t id = 0; id < MAX_PLAYERS; id++) {
        if ((self->bits[id] & PLAYER_BIT_ACTIVE) == 0) {
            continue;
        }

        if (id == world->player_id) {
            float speed = 100.0f;

            if (tq_is_key_pressed(TQ_KEY_LSHIFT)) {
                speed = 150.0f;
            }

            if (tq_is_key_pressed(TQ_KEY_UP) || tq_is_key_pressed(TQ_KEY_W)) {
                self->dy[id] -= speed;
                self->direction[id] = DIRECTION_UP;
            }

            if (tq_is_key_pressed(TQ_KEY_DOWN) || tq_is_key_pressed(TQ_KEY_S)) {
                self->dy[id] += speed;
                self->direction[id] = DIRECTION_DOWN;
            }

            if (tq_is_key_pressed(TQ_KEY_LEFT) || tq_is_key_pressed(TQ_KEY_A)) {
                self->dx[id] -= speed;
                self->direction[id] = DIRECTION_LEFT;
            }

            if (tq_is_key_pressed(TQ_KEY_RIGHT) || tq_is_key_pressed(TQ_KEY_D)) {
                self->dx[id] += speed;
                self->direction[id] = DIRECTION_RIGHT;
            }
        }

        self->x[id] += self->dx[id] * world->dt;

        if (self->dx[id] < 0.0f) {
            int p = (int) floorf((self->x[id] + 2.0f) / 16.0f);
            int aq = (int) floorf((self->y[id] + 2.0f) / 16.0f);
            int bq = (int) floorf((self->y[id] + 16.0f) / 16.0f);

            if (is_tile_solid(&world->stage, p, aq) || is_tile_solid(&world->stage, p, bq)) {
                self->x[id] = (float) p * 16.0f + 15.0f;
            }
        }

        if (self->dx[id] > 0.0f) {
            int p = (int) floorf((self->x[id] + 14.0f) / 16.0f);
            int aq = (int) floorf((self->y[id] + 2.0f) / 16.0f);
            int bq = (int) floorf((self->y[id] + 16.0f) / 16.0f);

            if (is_tile_solid(&world->stage, p, aq) || is_tile_solid(&world->stage, p, bq)) {
                self->x[id] = (float) p * 16.0f - 15.0f;
            }
        }

        self->y[id] += self->dy[id] * world->dt;

        if (self->dy[id] < 0.0f) {
            int ap = (int) floorf((self->x[id] + 2.0f) / 16.0f);
            int bp = (int) floorf((self->x[id] + 14.0f) / 16.0f);
            int q = (int) floorf((self->y[id] + 2.0f) / 16.0f);

            if (is_tile_solid(&world->stage, ap, q) || is_tile_solid(&world->stage, bp, q)) {
                self->y[id] = (float) q * 16.0f + 15.0f;
            }
        }

        if (self->dy[id] > 0.0f) {
            int ap = (int) floorf((self->x[id] + 2.0f) / 16.0f);
            int bp = (int) floorf((self->x[id] + 14.0f) / 16.0f);
            int q = (int) floorf((self->y[id] + 16.0f) / 16.0f);

            if (is_tile_solid(&world->stage, ap, q) || is_tile_solid(&world->stage, bp, q)) {
                self->y[id] = (float) q * 16.0f - 17.0f;
            }
        }

        if (self->dx[id] == 0.0f && self->dy[id] == 0.0f) {
            self->frame[id] = 1;
        } else {
            if (self->animation_tick[id] < world->ct) {
                self->frame[id] = (self->frame[id] + 1) % 3;
                self->animation_tick[id] = world->ct + 0.1f;
            }
        }

        self->dx[id] = 0.0f;
        self->dy[id] = 0.0f;
    }
}

void draw_players(players_t const *self)
{
    for (int32_t id = 0; id < MAX_PLAYERS; id++) {
        if ((self->bits[id] & PLAYER_BIT_VISIBLE) == 0) {
            continue;
        }

        float y_frame = 0;

        switch (self->direction[id]) {
        case DIRECTION_UP:
            y_frame = 0;
            break;
        case DIRECTION_DOWN:
            y_frame = 2;
            break;
        case DIRECTION_LEFT:
            y_frame = 3;
            break;
        case DIRECTION_RIGHT:
            y_frame = 1;
            break;
        }

        tq_push_matrix();
        tq_translate_matrix(TQ_VEC2F(self->x[id], self->y[id]));
        tq_outline_rectangle(TQ_RECTF(0.0f, 0.0f, 16.0f, 18.0f));
        tq_draw_subtexture(
            self->texture,
            TQ_RECTF(self->frame[id] * 16.0f, y_frame * 18.0f, 16.0f, 18.0f),
            TQ_RECTF(0.0f, 0.0f, 16.0f, 18.0f)
        );
        tq_pop_matrix();
    }
}

//------------------------------------------------------------------------------

int32_t spawn_object(objects_t *self, float x, float y)
{
    return 0;
}

void update_objects(world_t *world, objects_t *self)
{
}

void draw_objects(objects_t const *self)
{
}

//------------------------------------------------------------------------------

void spawn_world(world_t *world)
{
    memset(world, 0, sizeof(world_t));

    world->stage.texture = tq_load_texture_from_file("assets/jestan.png");
    world->players.texture = tq_load_texture_from_file("assets/raven.png");
    world->objects.texture = world->stage.texture;

    world->font = tq_load_font_from_file("assets/fonts/sansation-bold-italic.ttf",
        22.0f, TQ_FONT_NORMAL);

    spawn_stage(&world->stage);

    world->player_id = spawn_player(&world->players);
    world->players.x[world->player_id] = 80.0f;
    world->players.y[world->player_id] = 80.0f;
    world->players.direction[world->player_id] = DIRECTION_RIGHT;
}

void update_world(world_t *world)
{
    world->dt = (float) tq_get_delta_time();
    world->ct = world->ct + world->dt;

    update_stage(world, &world->stage);
    update_camera(world, &world->camera);
    update_players(world, &world->players);
    update_objects(world, &world->objects);
}

void draw_world(world_t const *world)
{
    tq_clear();

    tq_vec2i display_size = tq_get_display_size();

    float aspect = display_size.x / (float) display_size.y;
    
    if (!tq_is_key_pressed(TQ_KEY_P)) {
        if (aspect > 1.0f) {
            tq_set_view(
                TQ_RECTF(world->camera.x, world->camera.y, CAMERA_HEIGHT * aspect, CAMERA_HEIGHT),
                world->camera.rotation
            );
        } else {
            tq_set_view(
                TQ_RECTF(world->camera.x, world->camera.y, CAMERA_WIDTH, CAMERA_WIDTH / aspect),
                world->camera.rotation
            );
        }
    }

    tq_set_line_color(tq_c24(255, 255, 255));
    tq_set_outline_color(tq_c24(255, 255, 255));
    tq_set_fill_color(tq_c24(130, 170, 40));

    draw_stage(&world->stage);
    draw_objects(&world->objects);
    draw_players(&world->players);

    tq_draw_line(TQ_VEC2F(world->camera.x - 2.0f, world->camera.y), TQ_VEC2F(world->camera.x + 2.0f, world->camera.y));
    tq_draw_line(TQ_VEC2F(world->camera.x, world->camera.y - 2.0f), TQ_VEC2F(world->camera.x, world->camera.y + 2.0f));

    tq_set_fill_color(tq_c24(255, 255, 255));

    tq_vec2i cursor = tq_get_mouse_cursor_position();
    tq_vec2f mrel = tq_get_relative_position(tq_vec2i_cast(cursor));

    tq_vec2f prel = TQ_VEC2F(
        world->players.x[world->player_id],
        world->players.y[world->player_id]);

    tq_reset_view();

    tq_print_text(world->font, TQ_VEC2F(32, 80), "mouse abs: [%d, %d]", cursor.x, cursor.y);
    tq_print_text(world->font, TQ_VEC2F(32, 120), "mouse rel: [%.2f, %.2f]", mrel.x, mrel.y);
    tq_print_text(world->font, TQ_VEC2F(32, 160), "player rel: [%.2f, %.2f]", prel.x, prel.y);
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    tq_set_display_size(TQ_VEC2I(800, 600));
    tq_set_title("[tq library] jestan.c");

    tq_initialize();

    tq_set_auto_view_reset_enabled(false);
    tq_set_clear_color(tq_c24(0, 0, 0));

    world_t *world = malloc(sizeof(world_t));

    if (world) {
        spawn_world(world);

        while (tq_process()) {
            update_world(world);
            draw_world(world);
        }
    }

    tq_terminate();

    return 0;
}

//------------------------------------------------------------------------------
