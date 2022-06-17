
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <tq/tq.h>

//------------------------------------------------------------------------------

#define CANVAS_WIDTH    768
#define CANVAS_HEIGHT   768

//------------------------------------------------------------------------------
// Mandatory forward declarations

struct world;

//------------------------------------------------------------------------------
// Turret [Definition]

#define TURRET_LIMIT        4

#define TURRET_BIT_ACTIVE   0x01
#define TURRET_BIT_MANUAL   0x02
#define TURRET_BIT_TRIGGER  0x04

struct turrets
{
    uint32_t    bits[TURRET_LIMIT];
    float       x[TURRET_LIMIT];
    float       y[TURRET_LIMIT];
    float       rotation[TURRET_LIMIT];
    float       ideal_rotation[TURRET_LIMIT];
    float       base_rotation[TURRET_LIMIT];
    float       health[TURRET_LIMIT];
    float       ideal_health[TURRET_LIMIT];
    float       aim_x[TURRET_LIMIT];
    float       aim_y[TURRET_LIMIT];
    float       trigger_tick[TURRET_LIMIT];
    float       auto_think_tick[TURRET_LIMIT];
    int         auto_enemy[TURRET_LIMIT];
    float       recoil[TURRET_LIMIT];
    int         score[TURRET_LIMIT];
    float       bob[TURRET_LIMIT];

    int         current;
    tq_handle_t fire_sound;
};

//------------------------------------------------------------------------------
// Projectiles [Definition]

#define PROJECTILE_LIMIT        256
#define PROJECTILE_BIT_ACTIVE   0x01

struct projectiles
{
    uint32_t    bits[PROJECTILE_LIMIT];
    float       x[PROJECTILE_LIMIT];
    float       y[PROJECTILE_LIMIT];
    float       speed[PROJECTILE_LIMIT];
    float       rotation[PROJECTILE_LIMIT];
    float       trajectory[PROJECTILE_LIMIT];
    int         turret[PROJECTILE_LIMIT];
    float       bonus_tick[PROJECTILE_LIMIT];
    int         bonus[PROJECTILE_LIMIT];
    int         current;
};

int spawn_projectile_from_turret(struct world *world, struct projectiles *self, int turret);

//------------------------------------------------------------------------------
// Enemies [Definition]

#define ENEMY_LIMIT         64
#define ENEMY_BIT_ACTIVE    0x01
#define ENEMY_BIT_FRENZY    0x02

struct enemies
{
    uint32_t    bits[ENEMY_LIMIT];
    float       x[ENEMY_LIMIT];
    float       y[ENEMY_LIMIT];
    float       radius[ENEMY_LIMIT], ideal_radius[ENEMY_LIMIT];
    float       rotation[ENEMY_LIMIT], ideal_rotation[ENEMY_LIMIT];
    float       tx[ENEMY_LIMIT];
    float       ty[ENEMY_LIMIT];
    int         turret[ENEMY_LIMIT];
    float       spawn_tick;
    int         limit;
};

//------------------------------------------------------------------------------
// Particles [Definition]

#define PARTICLE_LIMIT          256
#define PARTICLE_BIT_ACTIVE     0x01
#define PARTICLE_TYPE_CIRCLE    0
#define PARTICLE_TYPE_SQUARE    1
#define PARTICLE_TYPE_POPUP     2

struct particles
{
    uint32_t    bits[PARTICLE_LIMIT];
    float       x[PARTICLE_LIMIT];
    float       y[PARTICLE_LIMIT];
    float       size[PARTICLE_LIMIT];
    float       ideal_size[PARTICLE_LIMIT];
    float       rate[PARTICLE_LIMIT];
    float       rotation[PARTICLE_LIMIT];
    int         type[PARTICLE_LIMIT];
    char        message[PARTICLE_LIMIT][32];
    int         current;
};

int spawn_circle_particle(struct particles *self, float x, float y, float size, float rate);
int spawn_square_particle(struct particles *self, float x, float y, float size, float rate);
int spawn_score_popup(struct particles *self, float x, float y, int score);
void update_particles(struct world *world, struct particles *self, float ct, float dt);
void draw_particles(struct particles const *self);

//------------------------------------------------------------------------------
// State [Definition]

#define STATE_BIT_ACTIVE        0x01
#define STATE_MODE_MAIN         0
#define STATE_MODE_PAUSE        1
#define STATE_MODE_TITLE        2
#define STATE_CONTROLS_PAUSE    0x01
#define STATE_CONTROLS_START    0x02

struct state
{
    uint32_t    bits;
    float       clock_tick;
    short       minutes;
    short       seconds;
    int         mode;
    int         controls;
    float       countdown_tick;
    float       restart_tick;
    float       tick;

    float       df0;
};

//------------------------------------------------------------------------------
// World [Definition]

struct world
{
    struct turrets          turrets;
    struct projectiles      projectiles;
    struct enemies          enemies;
    struct particles        particles;
    struct state            state;
};

//------------------------------------------------------------------------------
// UI [Definition]

void draw_crosshair(float x, float y);
void draw_string(float x, float y, float sx, float sy, int align, char const *fmt, ...);

//------------------------------------------------------------------------------
// Utilities

#define RADIANS(d) ((d) * (M_PI / 180.0f))
#define DEGREES(r) ((r) * (180.0f / M_PI))

float minf(float f0, float f1)
{
    return (f0 < f1) ? f0 : f1;
}

float maxf(float f0, float f1)
{
    return (f0 > f1) ? f0 : f1;
}

float distance(float ax, float ay, float bx, float by)
{
    return sqrtf(((by - ay) * (by - ay)) + ((bx - ax) * (bx - ax)));
}

float look_at(float x, float y, float target_x, float target_y)
{
    return DEGREES(atan2f(target_y - y, target_x - x));
}

void move(float *value, float ideal_value, float shrink_rate, float grow_rate, float dt)
{
    float const delta = *value - ideal_value;

    if (delta > 1.0f) {
        *value -= shrink_rate * dt;
    } else if (delta < -1.0f) {
        *value += grow_rate * dt;
    } else {
        *value = ideal_value;
    }
}

void rotate(float *rotation, float ideal_rotation, float speed, float dt)
{
    float const d = ideal_rotation - *rotation;
    float const r = fmodf(d + 540.0f, 360.0f) - 180.0f;

    if (r > 5.0f) {
        *rotation += speed * dt;
    } else if (r < -5.0f) {
        *rotation -= speed * dt;
    } else {
        *rotation = ideal_rotation;
    }

    *rotation = fmodf(*rotation + 180.0f, 360.0f) - 180.0f;
}

void move_to_angle(float *x, float *y, float rotation, float speed, float dt)
{
    float const dx = cosf(RADIANS(rotation));
    float const dy = sinf(RADIANS(rotation));

    *x += dx * speed * dt;
    *y += dy * speed * dt;
}

void random_point_in_a_circle(float x, float y, float radius, float *ox, float *oy)
{
    float const d = (rand() / (float) RAND_MAX) * radius;
    float const r = (rand() / (float) RAND_MAX) * M_PI * 2.0f;

    *ox = x + (cosf(r) * d);
    *oy = y + (sinf(r) * d);
}

void random_point_in_a_donut(float x, float y, float inner_radius, float outer_radius, float *ox, float *oy)
{
    float const h = outer_radius - inner_radius;
    float const d = inner_radius + (rand() / (float) RAND_MAX) * h;
    float const r = (rand() / (float) RAND_MAX) * M_PI * 2.0f;

    *ox = x + (cosf(r) * d);
    *oy = y + (sinf(r) * d);
}

float frand(float f)
{
    return ((float) rand() / RAND_MAX) * f;
}

//------------------------------------------------------------------------------
// Turrets [Implementation]

int spawn_turret(struct turrets *turrets)
{
    int id = turrets->current++;

    turrets->bits[id] = TURRET_BIT_ACTIVE;

    turrets->x[id] = 0.0f;
    turrets->y[id] = 0.0f;
    turrets->rotation[id] = turrets->ideal_rotation[id] = 0.0f;
    turrets->health[id] = turrets->ideal_health[id] = 100.0f;
    turrets->score[id] = 0;

    if (turrets->current == TURRET_LIMIT) {
        turrets->current = 0;
    }

    return id;
}

void turret_human_control(struct turrets *self, int id)
{
    self->aim_x[id] = (float) tq_get_mouse_cursor_x() - (CANVAS_WIDTH / 2.0f);
    self->aim_y[id] = (float) tq_get_mouse_cursor_y() - (CANVAS_HEIGHT / 2.0f);
    
    if (tq_is_mouse_button_pressed(TQ_MOUSE_BUTTON_LEFT)) {
        self->bits[id] |= TURRET_BIT_TRIGGER;
    } else {
        self->bits[id] &= ~TURRET_BIT_TRIGGER;
    }
}

int turret_auto_choose_enemy(struct turrets *self, int id, struct enemies const *enemies)
{
    int nearest_enemy = -1;
    float nearest_d = 0.0f;

    for (int enemy = 0; enemy < enemies->limit; enemy++) {
        if ((enemies->bits[enemy] & ENEMY_BIT_ACTIVE) == 0) {
            continue;
        }

        float const x = enemies->x[enemy];
        float const y = enemies->y[enemy];
        float const d = distance(self->x[id], self->y[id], x, y);

        if (nearest_enemy == -1) {
            nearest_enemy = enemy;
            nearest_d = d;
            continue;
        }

        if (d < nearest_d) {
            nearest_enemy = enemy;
            nearest_d = d;
        }
    }

    return nearest_enemy;
}

void turret_auto_control(struct turrets *self, int id, struct enemies const *enemies, float ct, float dt)
{
    if (ct > self->auto_think_tick[id]) {
        self->auto_enemy[id] = turret_auto_choose_enemy(self, id, enemies);
        self->auto_think_tick[id] = ct + 0.5f;
    }

    self->bits[id] &= ~TURRET_BIT_TRIGGER;

    if (self->auto_enemy[id] == -1) {
        self->auto_think_tick[id] = ct;
        return;
    }

    if ((enemies->bits[self->auto_enemy[id]] & ENEMY_BIT_ACTIVE) == 0) {
        self->auto_think_tick[id] = ct;
        return;
    }

    float const ex = enemies->x[self->auto_enemy[id]];
    float const ey = enemies->y[self->auto_enemy[id]];
    float const er = enemies->rotation[self->auto_enemy[id]];
    float const ed = distance(self->x[id], self->y[id], ex, ey);

    float const ax = ex + (cosf(RADIANS(er)) * ed * 0.1f);
    float const ay = ey + (sinf(RADIANS(er)) * ed * 0.1f);

    float const d = distance(ax, ay, self->aim_x[id], self->aim_y[id]);
    
    if (d < 2.0f) {
        self->aim_x[id] = ax;
        self->aim_y[id] = ay;
    } else {
        float const a = look_at(self->aim_x[id], self->aim_y[id], ax, ay);
        float const sx = 300.0f + (3000.0f * (d / CANVAS_WIDTH));
        float const sy = 240.0f + (2400.0f * (d / CANVAS_HEIGHT));
        self->aim_x[id] += cosf(RADIANS(a)) * sx * dt;
        self->aim_y[id] += sinf(RADIANS(a)) * sy * dt;
    }

    float const a = look_at(self->x[id], self->y[id], ex, ey);

    if (fabsf(self->rotation[id] - a) < 30.0f) {
        if ((d < 128.0f) || (self->recoil[id] < 100.0f)) {
            self->bits[id] |= TURRET_BIT_TRIGGER;
        }
    }
}

void update_turrets(struct world *world, struct turrets *self, float ct, float dt)
{
    for (int id = 0; id < TURRET_LIMIT; id++) {
        if ((self->bits[id] & TURRET_BIT_ACTIVE) == 0) {
            continue;
        }

        if (self->bits[id] & TURRET_BIT_MANUAL) {
            turret_human_control(self, id);
        } else {
            turret_auto_control(self, id, &world->enemies, ct, dt);
        }

        self->ideal_rotation[id] = look_at(self->x[id], self->y[id], self->aim_x[id], self->aim_y[id]);

        float const rs = 360.0f; // level.turret_rotation_speed[level.current];
        rotate(&self->rotation[id], self->ideal_rotation[id], rs, dt);
        rotate(&self->base_rotation[id], self->ideal_rotation[id], rs * 0.66f, dt);

        if (self->bits[id] & TURRET_BIT_TRIGGER) {
            if (ct > self->trigger_tick[id]) {
                float const d = distance(self->x[id], self->y[id], self->aim_x[id], self->aim_y[id]);
                float const v = maxf(d / 128.0f, 1.0f);
    
                self->bob[id] = 7.0f;

                spawn_projectile_from_turret(world, &world->projectiles, id);
                tq_play_sound(self->fire_sound, 1.0f, 0.0f, 0);

                self->trigger_tick[id] = ct + (0.125f * v);
                self->recoil[id] = minf(self->recoil[id] + 400.0f * dt, 300.0f / v);
            }
        } else {
            self->recoil[id] = maxf(0.0f, self->recoil[id] + 60.0f * dt);
        }

        move(&self->bob[id], 0.0f, 50.0f, 50.0f, dt);
        move(&self->health[id], self->ideal_health[id], 50.0f, 25.0f, dt);

        if (self->health[id] <= 0.0f) {
            self->bits[id] &= ~TURRET_BIT_ACTIVE;
            
            spawn_square_particle(&world->particles, self->x[id], self->y[id], 48.0f, 100.0f);
            spawn_square_particle(&world->particles, self->x[id], self->y[id], 40.0f, 75.0f);
            spawn_square_particle(&world->particles, self->x[id], self->y[id], 32.0f, 50.0f);
            spawn_square_particle(&world->particles, self->x[id], self->y[id], 24.0f, 25.0f);
        }

        if (self->ideal_health[id] < 100.0f) {
            self->ideal_health[id] += 1.0f * dt;

            if (self->ideal_health[id] > 100.0f) {
                self->ideal_health[id] = 100.0f;
            }
        }
    }
}

void draw_turrets(struct turrets const *self)
{
    for (int id = 0; id < TURRET_LIMIT; id++) {
        if ((self->bits[id] & TURRET_BIT_ACTIVE) == 0) {
            continue;
        }

        tq_push_matrix();
            tq_translate_matrix_f(self->x[id], self->y[id]);
            tq_push_matrix();
                tq_rotate_matrix(self->base_rotation[id]);
                tq_draw_triangle_f(-10.0f, -10.0f, -10.0f, 10.0f, -20.0f, 0.0f);
            tq_pop_matrix();
            tq_draw_circle_f(0.0f, 0.0f, 16.0f);
            tq_rotate_matrix(self->rotation[id]);
            tq_draw_rectangle_f(-10.0f - self->bob[id], -5.0f, 40.0f, 10.0f);
            tq_draw_rectangle_f(-10.0f - self->bob[id], -5.0f, 10.0f, 10.0f);
        tq_pop_matrix();

        if (self->bits[id] & TURRET_BIT_MANUAL) {
            draw_crosshair(self->aim_x[id], self->aim_y[id]);
        }
    }
}

//------------------------------------------------------------------------------
// Projectiles [Implementation]

int spawn_projectile_from_turret(struct world *world, struct projectiles *self, int turret)
{
    int id = self->current++;

    float const x = world->turrets.x[turret];
    float const y = world->turrets.y[turret];
    float const r = world->turrets.rotation[turret];
    float const h = -0.5f + ((float) rand() / (float) RAND_MAX);

    self->bits[id] = PROJECTILE_BIT_ACTIVE;
    self->x[id] = x + (cosf(RADIANS(r)) * 20.0f);
    self->y[id] = y + (sinf(RADIANS(r)) * 20.0f);
    self->rotation[id] = r;
    self->trajectory[id] = h * world->turrets.recoil[turret];
    self->turret[id] = turret;
    self->speed[id] = 800.0f;
    self->bonus[id] = 0;
    self->bonus_tick[id] = 0.0f;

    if (self->current == PROJECTILE_LIMIT) {
        self->current = 0;
    }

    return id;
}

void projectile_vs_turrets(struct world *world, struct projectiles *self, int id, float ct, float dt)
{
    for (int turret = 0; turret < TURRET_LIMIT; turret++) {
        if ((world->turrets.bits[turret] & TURRET_BIT_ACTIVE) == 0) {
            continue;
        }

        float const x = world->turrets.x[turret];
        float const y = world->turrets.y[turret];
        float const d = distance(self->x[id], self->y[id], x, y);

        if (d < 16.0f) {
            self->bits[id] &= ~PROJECTILE_BIT_ACTIVE;
            spawn_circle_particle(&world->particles, self->x[id], self->y[id], 8.0f, 40.0f);
        }
    }
}

void projectile_vs_enemies(struct world *world, struct projectiles *self, int id, float ct, float dt)
{
    for (int enemy = 0; enemy < ENEMY_LIMIT; enemy++) {
        if ((world->enemies.bits[enemy] & ENEMY_BIT_ACTIVE) == 0) {
            continue;
        }

        float const x = world->enemies.x[enemy];
        float const y = world->enemies.y[enemy];
        float const d = distance(self->x[id], self->y[id], x, y);

        if (d < world->enemies.radius[enemy]) {
            self->bits[id] &= ~PROJECTILE_BIT_ACTIVE;

            world->enemies.bits[enemy] |= ENEMY_BIT_FRENZY;
            world->enemies.ideal_radius[enemy] -= 5.0f;
            world->enemies.turret[enemy] = self->turret[id];

            float const ox = world->turrets.x[self->turret[id]];
            float const oy = world->turrets.y[self->turret[id]];
            float const od = distance(self->x[id], self->y[id], ox, oy);
            int const score = 10 * (int) (od / 10.0f);

            world->turrets.score[self->turret[id]] += score;

            spawn_circle_particle(&world->particles, self->x[id], self->y[id], 8.0f, 40.0f);
            spawn_score_popup(&world->particles, self->x[id], self->y[id] + 8.0f, score);
        }
    }
}

void update_projectiles(struct world *world, struct projectiles *self, float ct, float dt)
{
    for (int id = 0; id < PROJECTILE_LIMIT; id++) {
        if ((self->bits[id] & PROJECTILE_BIT_ACTIVE) == 0) {
            continue;
        }

        self->rotation[id] += self->trajectory[id] * dt;
        self->trajectory[id] -= self->trajectory[id] * 4.0f * dt;

        if (self->bonus_tick[id] < ct) {
            self->bonus[id] += 100;
            self->bonus_tick[id] = ct + 0.0625f;
        }

        int const num_steps = 8;
        float const step = self->speed[id] / num_steps;

        for (int n = 0; n < num_steps; n++) {
            move_to_angle(&self->x[id], &self->y[id], self->rotation[id], step, dt);

            projectile_vs_enemies(world, self, id, ct, dt);
            projectile_vs_turrets(world, self, id, ct, dt);

            if ((self->bits[id] & PROJECTILE_BIT_ACTIVE) == 0) {
                break;
            }
        }

        if ((self->x[id] < -(CANVAS_WIDTH / 2.0f)) || (self->x[id] > (CANVAS_WIDTH / 2.0f))) {
            self->bits[id] &= ~PROJECTILE_BIT_ACTIVE;
        }

        if ((self->y[id] < -(CANVAS_HEIGHT / 2.0f)) || (self->y[id] > (CANVAS_HEIGHT / 2.0f))) {
            self->bits[id] &= ~PROJECTILE_BIT_ACTIVE;
        }
    }
}

void draw_projectiles(struct projectiles const *projectiles)
{
    for (int id = 0; id < PROJECTILE_LIMIT; id++) {
        if ((projectiles->bits[id] & PROJECTILE_BIT_ACTIVE) == 0) {
            continue;
        }

        tq_push_matrix();
            tq_translate_matrix_f(projectiles->x[id], projectiles->y[id]);
            tq_rotate_matrix(projectiles->rotation[id]);
            tq_draw_line_f(-8.0f, 0.0f, 8.0f, 0.0f);
        tq_pop_matrix();
    }
}

//------------------------------------------------------------------------------
// Enemies [Implementation]

int spawn_enemy(struct enemies *self)
{
    for (int id = 0; id < self->limit; id++) {
        if ((self->bits[id] & ENEMY_BIT_ACTIVE) == 0) {
            self->bits[id] = ENEMY_BIT_ACTIVE;
            return id;
        }
    }

    return -1;
}

void enemy_choose_target(struct enemies *self, int id)
{
    random_point_in_a_donut(0.0f, 0.0f, 256.0f, 512.0f, &self->tx[id], &self->ty[id]);
}

void update_enemies(struct world *world, struct enemies *self, float ct, float dt)
{
    if (ct > self->spawn_tick) {
        int id = spawn_enemy(self);
        
        if (id != -1) {
            self->radius[id] = 8.0f;
            self->ideal_radius[id] = 24.0f;
            self->rotation[id] = rand() % 360;

            random_point_in_a_donut(0.0f, 0.0f, 700.0f, 1000.0f, &self->x[id], &self->y[id]);
            enemy_choose_target(self, id);
        }

        self->spawn_tick = ct + 0.125f /* level.enemy_respawn_timeout[level.current] */;
    }

    for (int id = 0; id < self->limit; id++) {
        if ((self->bits[id] & ENEMY_BIT_ACTIVE) == 0) {
            continue;
        }

        move(&self->radius[id], self->ideal_radius[id], 100.0f, 25.0f, dt);

        if (self->radius[id] < 8.0f) {
            self->bits[id] &= ~ENEMY_BIT_ACTIVE;
            continue;
        }

        if (self->bits[id] & ENEMY_BIT_FRENZY) {
            float const x = world->turrets.x[self->turret[id]];
            float const y = world->turrets.y[self->turret[id]];
            self->ideal_rotation[id] = look_at(self->x[id], self->y[id], x, y);

            if ((world->turrets.bits[self->turret[id]] & TURRET_BIT_ACTIVE) == 0) {
                self->bits[id] &= ~ENEMY_BIT_FRENZY;
            }
        } else {
            self->ideal_rotation[id] = look_at(self->x[id], self->y[id], self->tx[id], self->ty[id]);
        }

        rotate(&self->rotation[id], self->ideal_rotation[id], 200.0f, dt);
        move_to_angle(&self->x[id], &self->y[id], self->rotation[id], 40.0f, dt);

        float const d = distance(self->x[id], self->y[id], self->tx[id], self->ty[id]);
        if (d < self->radius[id]) {
            enemy_choose_target(self, id);
        }

        for (int turret = 0; turret < TURRET_LIMIT; turret++) {
            if ((world->turrets.bits[turret] & TURRET_BIT_ACTIVE) == 0) {
                continue;
            }

            float const x = world->turrets.x[turret];
            float const y = world->turrets.y[turret];
            float const d = distance(self->x[id], self->y[id], x, y);
        
            if (d < (self->radius[id] + 16.0f)) {
                self->ideal_radius[id] = 0.0f;
                world->turrets.ideal_health[turret] -= 20.0f;
            }
        }
    }
}

void draw_enemies(struct enemies const *self)
{
    for (int id = 0; id < self->limit; id++) {
        if ((self->bits[id] & ENEMY_BIT_ACTIVE) == 0) {
            continue;
        }

        float const r = self->radius[id];

        tq_push_matrix();
        tq_translate_matrix_f(self->x[id], self->y[id]);
        tq_rotate_matrix(self->rotation[id]);
        tq_draw_triangle_f(0.0f, -r, 0.0f, r, r * 1.5f, 0.0f);
        tq_draw_circle_f(0.0f, 0.0f, self->radius[id]);
        tq_pop_matrix();
    }
}

//------------------------------------------------------------------------------
// Square [Implementation]

int spawn_circle_particle(struct particles *self, float x, float y, float size, float rate)
{
    int id = self->current++;

    self->bits[id] = PARTICLE_BIT_ACTIVE;
    self->x[id] = x;
    self->y[id] = y;
    self->size[id] = 0.0f;
    self->ideal_size[id] = size;
    self->rate[id] = rate;
    self->rotation[id] = 0.0f;
    self->type[id] = PARTICLE_TYPE_CIRCLE;
    self->message[id][0] = '\0';

    if (self->current == PARTICLE_LIMIT) {
        self->current = 0;
    }

    return id;
}

int spawn_square_particle(struct particles *self, float x, float y, float size, float rate)
{
    int id = self->current++;

    self->bits[id] = PARTICLE_BIT_ACTIVE;
    self->x[id] = x;
    self->y[id] = y;
    self->size[id] = 0.0f;
    self->ideal_size[id] = size;
    self->rate[id] = rate;
    self->rotation[id] = 360.0f * (rand() / (float) RAND_MAX);
    self->type[id] = PARTICLE_TYPE_SQUARE;
    self->message[id][0] = '\0';

    if (self->current == PARTICLE_LIMIT) {
        self->current = 0;
    }

    return id;
}

int spawn_score_popup(struct particles *self, float x, float y, int score)
{
    int id = self->current++;

    self->bits[id] = PARTICLE_BIT_ACTIVE;
    self->x[id] = x;
    self->y[id] = y;
    self->size[id] = 12.0f;
    self->ideal_size[id] = 16.0f;
    self->rate[id] = 15.0f;
    self->rotation[id] = 360.0f * (rand() / (float) RAND_MAX);
    self->type[id] = PARTICLE_TYPE_POPUP;
    snprintf(self->message[id], sizeof(self->message[id]), "%d", score);

    if (self->current == PARTICLE_LIMIT) {
        self->current = 0;
    }

    return id;
}

void update_particles(struct world *world, struct particles *self, float ct, float dt)
{
    for (int id = 0; id < PARTICLE_LIMIT; id++) {
        if ((self->bits[id] & PARTICLE_BIT_ACTIVE) == 0) {
            continue;
        }

        self->size[id] += self->rate[id] * dt;

        if (self->type[id] == PARTICLE_TYPE_SQUARE) {
            self->rotation[id] += self->rate[id] * dt;
        }

        if (self->type[id] == PARTICLE_TYPE_POPUP) {
            self->y[id] -= self->rate[id] * 4.0f * dt;
        }

        if (self->size[id] > self->ideal_size[id]) {
            self->bits[id] &= ~PARTICLE_BIT_ACTIVE;
        }
    }
}

void draw_particles(struct particles const *self)
{
    for (int id = 0; id < PARTICLE_LIMIT; id++) {
        if ((self->bits[id] & PARTICLE_BIT_ACTIVE) == 0) {
            continue;
        }

        if (self->type[id] == PARTICLE_TYPE_CIRCLE) {
            tq_outline_circle_f(self->x[id], self->y[id], self->size[id]);
        } else if (self->type[id] == PARTICLE_TYPE_SQUARE) {
            float const s = self->size[id];
            tq_push_matrix();
                tq_translate_matrix_f(self->x[id], self->y[id]);
                tq_rotate_matrix(self->rotation[id]);
                tq_outline_rectangle_f(-s / 2, -s / 2, s, s);
            tq_pop_matrix();
        } else if (self->type[id] == PARTICLE_TYPE_POPUP) {
            float const s = self->size[id];
            draw_string(self->x[id], self->y[id], s, s * 1.5f, 0, "%s", self->message[id]);
        }
    }
}

//------------------------------------------------------------------------------
// World [Implementation]

void reset_game(struct world *world)
{
    for (int turret = 0; turret < TURRET_LIMIT; turret++) {
        world->turrets.bits[turret] = 0;
    }

    for (int projectile = 0; projectile < PROJECTILE_LIMIT; projectile++) {
        world->projectiles.bits[projectile] = 0;
    }

    for (int enemy = 0; enemy < ENEMY_LIMIT; enemy++) {
        world->enemies.bits[enemy] = 0;
    }

    for (int particle = 0; particle < PARTICLE_LIMIT; particle++) {
        world->particles.bits[particle] = 0;
    }

    world->turrets.current = 0;
    world->projectiles.current = 0;
    world->particles.current = 0;
    world->enemies.limit = 16;

    world->state.minutes = 0;
    world->state.seconds = -1;
    world->state.clock_tick = 0.0f;
}

void switch_to_game(struct world *world)
{
    reset_game(world);

    world->state.bits = STATE_BIT_ACTIVE;
    world->state.mode = STATE_MODE_MAIN;

    int player = spawn_turret(&world->turrets);

    world->turrets.x[player] = 0.0f;
    world->turrets.y[player] = 0.0f;
    world->turrets.bits[player] |= TURRET_BIT_MANUAL;
}

void switch_to_title(struct world *world)
{
    reset_game(world);

    world->state.bits = STATE_BIT_ACTIVE;
    world->state.mode = STATE_MODE_TITLE;

    int alpha = spawn_turret(&world->turrets);
    world->turrets.x[alpha] = -200.0f;
    world->turrets.y[alpha] = 0.0f;

    int beta = spawn_turret(&world->turrets);
    world->turrets.x[beta] = 200.0f;
    world->turrets.y[beta] = 0.0f;
}

void set_color_theme(tq_color_t background, tq_color_t foreground)
{
    tq_set_clear_color(background);
    tq_set_fill_color(background);

    tq_set_point_color(foreground);
    tq_set_line_color(foreground);
    tq_set_outline_color(foreground);
}

void spawn_world(struct world *world)
{
    // set_color_theme(tq_rgb(40, 40, 40), tq_rgb(192, 192, 192));
    set_color_theme(tq_rgb(28, 28, 40), tq_rgb(192, 160, 192));

    world->turrets.fire_sound = tq_load_sound_from_file("assets/defense/turret_attack.ogg");

    switch_to_title(world);
}

void update_world(struct world *world)
{
    switch (world->state.mode) {
    case STATE_MODE_MAIN:
        if (tq_is_key_pressed(TQ_KEY_ESCAPE)) {
            if ((world->state.controls & STATE_CONTROLS_PAUSE) == 0) {
                world->state.controls |= STATE_CONTROLS_PAUSE;
                world->state.mode = STATE_MODE_PAUSE;
            }
        } else {
            world->state.controls &= ~STATE_CONTROLS_PAUSE;
        }
        break;
    case STATE_MODE_PAUSE:
        if (tq_is_key_pressed(TQ_KEY_ESCAPE)) {
            if ((world->state.controls & STATE_CONTROLS_PAUSE) == 0) {
                world->state.controls |= STATE_CONTROLS_PAUSE;
                world->state.mode = STATE_MODE_MAIN;
            }
        } else {
            world->state.controls &= ~STATE_CONTROLS_PAUSE;
        }
        break;
    case STATE_MODE_TITLE:
        if (tq_is_key_pressed(TQ_KEY_SPACE)) {
            if ((world->state.controls & STATE_CONTROLS_START) == 0) {
                world->state.controls |= STATE_CONTROLS_START;
                world->state.mode = STATE_MODE_MAIN;
                switch_to_game(world);
            }
        } else {
            world->state.controls &= ~STATE_CONTROLS_START;
        }
        break;
    }

    if (world->state.mode == STATE_MODE_PAUSE) {
        return;
    }

    float const time_scale = 1.00f;
    float dt = (float) tq_get_delta_time() * time_scale;
    world->state.tick += dt;

    if (world->state.bits & STATE_BIT_ACTIVE) {
        if (world->state.tick > world->state.clock_tick) {
            world->state.seconds++;

            if (world->state.seconds == 60) {
                world->state.seconds = 0;
                world->state.minutes++;

                if (world->state.minutes == 60) {
                    world->state.minutes = 0;
                }
            }

            world->state.clock_tick = world->state.tick + 1.0f;
        }
    }

    update_turrets(world, &world->turrets, world->state.tick, dt);
    update_projectiles(world, &world->projectiles, world->state.tick, dt);
    update_enemies(world, &world->enemies, world->state.tick, dt);
    update_particles(world, &world->particles, world->state.tick, dt);

    if (world->state.bits & STATE_BIT_ACTIVE) {
        if ((world->turrets.bits[0] & TURRET_BIT_ACTIVE) == 0) {
            world->state.bits &= ~STATE_BIT_ACTIVE;
            world->state.restart_tick = world->state.tick + 1.0f;
        }
    } else {
        if (world->state.tick > world->state.restart_tick) {
            if (tq_is_key_pressed(TQ_KEY_SPACE)) {
                if ((world->state.controls & STATE_CONTROLS_START) == 0) {
                    world->state.controls |= STATE_CONTROLS_START;
                    switch_to_title(world);
                }
            } else {
                world->state.controls &= ~STATE_CONTROLS_START;
            }
        }
    }
}

void draw_world(struct world const *world)
{
    tq_clear();

    tq_push_matrix();
        tq_translate_matrix_f(CANVAS_WIDTH / 2.0f, CANVAS_HEIGHT / 2.0f);
        draw_turrets(&world->turrets);
        draw_projectiles(&world->projectiles);
        draw_enemies(&world->enemies);
        draw_particles(&world->particles);
    tq_pop_matrix();

    switch (world->state.mode) {
    case STATE_MODE_PAUSE:
        draw_string(CANVAS_WIDTH / 2.0f, CANVAS_HEIGHT / 3.0f, 24.0f, 48.0f, 0, "pause");
        /* intentional fall-through */
    case STATE_MODE_MAIN:
        draw_string(24.0f, 24.0f, 16.0f, 24.0f, -1, "%09d", (world->turrets.score[0]));
        draw_string(CANVAS_WIDTH - 24.0f, 24.0f,
            16.0f, 24.0f,
            1, "%02d:%02d", world->state.minutes, world->state.seconds);

        tq_draw_line_f(
            0.0f,
            CANVAS_HEIGHT - 1.0f,
            CANVAS_WIDTH * (world->turrets.health[0] / 100.0f),
            CANVAS_HEIGHT - 1.0f
        );
        break;
    case STATE_MODE_TITLE:
        draw_string(CANVAS_WIDTH / 2.0f, CANVAS_HEIGHT * 0.25f, 40.0f, 80.0f, 0, "defense");
        draw_string(CANVAS_WIDTH / 2.0f, CANVAS_HEIGHT * 0.75f, 16.0f, 24.0f, 0, "press space to play");
        break;
    default:
        break;
    }
}

//------------------------------------------------------------------------------
// UI [Implementation]

void draw_crosshair(float x, float y)
{
    tq_draw_line_f(x - 8.0f, y, x + 8.0f, y);
    tq_draw_line_f(x, y - 8.0f, x, y + 8.0f);
}

void draw_string(float x, float y, float sx, float sy, int align, char const *fmt, ...)
{
    static float const lines[256][32] = {
        ['0'] = {
            0.00f, 0.00f, 1.00f, 0.00f,
            1.00f, 0.00f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.00f,
        },
        ['1'] = {
            0.00f, 0.00f, 0.50f, 0.00f,
            0.50f, 0.00f, 0.50f, 0.75f,
            0.00f, 0.75f, 1.00f, 0.75f,
        },
        ['2'] = {
            0.00f, 0.00f, 1.00f, 0.00f,
            1.00f, 0.00f, 1.00f, 0.25f,
            1.00f, 0.25f, 0.00f, 0.25f,
            0.00f, 0.25f, 0.00f, 0.75f,
            0.00f, 0.75f, 1.00f, 0.75f,
        },
        ['3'] = {
            0.00f, 0.00f, 1.00f, 0.00f,
            1.00f, 0.00f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.25f, 1.00f, 0.25f,
        },
        ['4'] = {
            0.00f, 0.00f, 0.00f, 0.50f,
            0.00f, 0.50f, 1.00f, 0.50f,
            1.00f, 0.00f, 1.00f, 0.75f,
        },
        ['5'] = {
            0.00f, 0.00f, 1.00f, 0.00f,
            0.00f, 0.00f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
            0.00f, 0.75f, 1.00f, 0.75f,
        },
        ['6'] = {
            0.00f, 0.00f, 0.00f, 0.75f,
            0.00f, 0.00f, 1.00f, 0.00f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
            0.00f, 0.75f, 1.00f, 0.75f,
        },
        ['7'] = {
            0.00f, 0.00f, 1.00f, 0.00f,
            0.00f, 0.00f, 0.00f, 0.25f,
            1.00f, 0.00f, 1.00f, 0.75f,
        },
        ['8'] = {
            0.00f, 0.00f, 1.00f, 0.00f,
            1.00f, 0.00f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.00f,
            0.00f, 0.25f, 1.00f, 0.25f,
        },
        ['9'] = {
            0.00f, 0.00f, 1.00f, 0.00f,
            0.00f, 0.00f, 0.00f, 0.50f,
            1.00f, 0.00f, 1.00f, 0.75f,
            0.00f, 0.50f, 1.00f, 0.50f,
            0.00f, 0.75f, 1.00f, 0.75f,
        },
        [':'] = {
            0.00f, 0.25f, 0.00f, 0.30f,
            0.00f, 0.50f, 0.00f, 0.55f,
        },
        ['a'] = {
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.50f,
            0.00f, 0.50f, 1.00f, 0.50f,
        },
        ['b'] = {
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.00f,
        },
        ['c'] = {
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
        },
        ['d'] = {
            1.00f, 0.00f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
        },
        ['e'] = {
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.50f,
            1.00f, 0.50f, 0.00f, 0.50f,
        },
        ['f'] = {
            0.50f, 0.75f, 0.50f, 0.00f,
            0.50f, 0.00f, 1.00f, 0.00f,
            0.00f, 0.25f, 1.00f, 0.25f,
        },
        ['g'] = {
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 1.00f,
            1.00f, 1.00f, 0.00f, 1.00f,
        },
        ['h'] = {
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
            0.00f, 0.00f, 0.00f, 0.75f,
        },
        ['i'] = {
            0.00f, 0.00f, 0.00f, 0.06f,
            0.00f, 0.25f, 0.00f, 0.75f,
        },
        ['j'] = {
            0.25f, 0.00f, 0.25f, 0.06f,
            0.25f, 0.25f, 0.25f, 1.00f,
            0.25f, 1.00f, -0.25f, 1.00f,
        },
        ['k'] = {
            0.00f, 0.00f, 0.00f, 0.75f,
            0.00f, 0.50f, 1.00f, 0.25f,
            0.00f, 0.50f, 1.00f, 0.75f,
        },
        ['l'] = {
            0.00f, 0.00f, 0.00f, 0.75f,
        },
        ['m'] = {
            0.00f, 0.25f, 1.50f, 0.25f,
            0.00f, 0.25f, 0.00f, 0.75f,
            0.75f, 0.25f, 0.75f, 0.75f,
            1.50f, 0.25f, 1.50f, 0.75f,
        },
        ['n'] = {
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
        },
        ['o'] = {
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
        },
        ['p'] = {
            0.00f, 1.00f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
        },
        ['q'] = {
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 1.00f, 1.00f,
        },
        ['r'] = {
            0.00f, 0.75f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
        },
        ['s'] = {
            0.00f, 0.50f, 0.00f, 0.25f,
            0.00f, 0.25f, 1.00f, 0.25f,
            0.00f, 0.50f, 1.00f, 0.50f,
            1.00f, 0.50f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
        },
        ['t'] = {
            1.00f, 0.75f, 0.50f, 0.75f,
            0.50f, 0.75f, 0.50f, 0.00f,
            0.00f, 0.25f, 1.00f, 0.25f,
        },
        ['u'] = {
            1.00f, 0.25f, 1.00f, 0.75f,
            1.00f, 0.75f, 0.00f, 0.75f,
            0.00f, 0.75f, 0.00f, 0.25f,
        },
        ['v'] = {
            1.00f, 0.25f, 0.50f, 0.75f,
            0.50f, 0.75f, 0.00f, 0.25f,
        },
        ['w'] = {
            0.000f, 0.250f, 0.375f, 0.750f,
            0.375f, 0.750f, 0.750f, 0.375f,
            0.750f, 0.375f, 1.125f, 0.750f,
            1.125f, 0.750f, 1.500f, 0.250f,
        },
        ['x'] = {
            0.00f, 0.25f, 1.00f, 0.75f,
            1.00f, 0.25f, 0.00f, 0.75f,
        },
        ['y'] = {
            0.00f, 0.25f, 0.00f, 0.75f,
            0.00f, 0.75f, 1.00f, 0.75f,
            1.00f, 0.25f, 1.00f, 1.00f,
            1.00f, 1.00f, 0.00f, 1.00f,
        },
        ['z'] = {
            0.00f, 0.25f, 1.00f, 0.25f,
            1.00f, 0.25f, 0.00f, 0.75f,
            0.00f, 0.75f, 1.00f, 0.75f,
        },
    };

    static int const total_lines[256] = {
        ['0'] = 4,
        ['1'] = 3,
        ['2'] = 5,
        ['3'] = 4,
        ['4'] = 3,
        ['5'] = 5,
        ['6'] = 5,
        ['7'] = 3,
        ['8'] = 5,
        ['9'] = 5,
        [':'] = 2,
        ['a'] = 5,
        ['b'] = 4,
        ['c'] = 3,
        ['d'] = 4,
        ['e'] = 5,
        ['f'] = 3,
        ['g'] = 5,
        ['h'] = 3,
        ['i'] = 2,
        ['j'] = 3,
        ['k'] = 3,
        ['l'] = 1,
        ['m'] = 4,
        ['n'] = 3,
        ['o'] = 4,
        ['p'] = 4,
        ['q'] = 4,
        ['r'] = 2,
        ['s'] = 5,
        ['t'] = 3,
        ['u'] = 3,
        ['v'] = 2,
        ['w'] = 4,
        ['x'] = 2,
        ['y'] = 4,
        ['z'] = 3,
    };

    static float const widths[256] = {
        ['0'] = 1.00f,
        ['1'] = 1.00f,
        ['2'] = 1.00f,
        ['3'] = 1.00f,
        ['4'] = 1.00f,
        ['5'] = 1.00f,
        ['6'] = 1.00f,
        ['7'] = 1.00f,
        ['8'] = 1.00f,
        ['9'] = 1.00f,
        [':'] = 0.00f,
        ['a'] = 1.00f,
        ['b'] = 1.00f,
        ['c'] = 1.00f,
        ['d'] = 1.00f,
        ['e'] = 1.00f,
        ['f'] = 1.00f,
        ['g'] = 1.00f,
        ['h'] = 1.00f,
        ['i'] = 0.00f,
        ['j'] = 0.25f,
        ['k'] = 1.00f,
        ['l'] = 0.00f,
        ['m'] = 1.50f,
        ['n'] = 1.00f,
        ['o'] = 1.00f,
        ['p'] = 1.00f,
        ['q'] = 1.00f,
        ['r'] = 1.00f,
        ['s'] = 1.00f,
        ['t'] = 1.00f,
        ['u'] = 1.00f,
        ['v'] = 1.00f,
        ['w'] = 1.50f,
        ['x'] = 1.00f,
        ['y'] = 1.00f,
        ['z'] = 1.00f,
    };

    float const rx = 0.5f;
    float const ry = 1.0f;
    float const k = 0.5f;

    char buffer[1024];
    va_list args;

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    float cx = 0.0f;
    float cy = y;

    if (align < 0) {
        cx = x;
    } else {
        float length = 0.0f;

        for (char const *s = buffer; *s; *s++) {
            length += sx * (widths[*s] + k);
        }

        if (align == 0) {
            cx = x - (length / 2.0f);
        } else {
            cx = x - length;
        }
    }

    for (char const *s = buffer; *s; *s++) {
        for (int i = 0; i < total_lines[*s]; i++) {
            tq_draw_line_f(
                cx + (frand(rx) * 2.0f - rx) + (lines[*s][4 * i + 0] * sx),
                cy + (frand(ry) * 2.0f - ry) + (lines[*s][4 * i + 1] * sy),
                cx + (frand(rx) * 2.0f - rx) + (lines[*s][4 * i + 2] * sx),
                cy + (frand(ry) * 2.0f - ry) + (lines[*s][4 * i + 3] * sy)
            );
        }

        cx += sx * (widths[*s] + k);
    }
}

//------------------------------------------------------------------------------
// Entry point

int main(int argc, char *argv[])
{
    tq_set_display_size(CANVAS_WIDTH, CANVAS_HEIGHT);
    tq_set_title("[tq library] defense.c");

    tq_initialize();

    {
        struct world world = { 0 };
        spawn_world(&world);

        while (tq_process()) {
            update_world(&world);
            draw_world(&world);
        }
    }

    tq_terminate();
    return 0;
}

//------------------------------------------------------------------------------
