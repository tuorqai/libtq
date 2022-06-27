
//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENGL)

//------------------------------------------------------------------------------

#include <string.h>

#if !defined(TQ_USE_OPENGL_ES)
#   include <GL/glew.h>
#else
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#endif

#include "tq_core.h"
#include "tq_image_loader.h"
#include "tq_log.h"
#include "tq_math.h"
#include "tq_renderer.h"

//------------------------------------------------------------------------------
// OpenGL debug stuff

#if defined(NDEBUG)

#define CHECK_GL(call_) call_

#else

static char const *get_gl_error_str(GLenum error)
{
    switch (error) {
    case GL_NO_ERROR:           return "No error, everything is OK.";
    case GL_INVALID_ENUM:       return "Invalid enumeration.";
    case GL_INVALID_VALUE:      return "Invalid value.";
    case GL_INVALID_OPERATION:  return "Invalid operation.";
    case GL_OUT_OF_MEMORY:      return "Out of memory.";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
                                return "Invalid framebuffer operation.";
    }

    return "Unknown error.";
}

static void check_gl_errors(char const *call, char const *file, unsigned int line)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        log_error("OpenGL error(s) occured in %s at line %d:\n", file, line);
        log_error("-- %d: %s\n", line, call);
    }

    while (error != GL_NO_ERROR) {
        log_error(":: [0x%x] %s\n", error, get_gl_error_str(error));
        error = glGetError();
    }
}

#define CHECK_GL(call_) \
    do { \
        call_; \
        check_gl_errors(#call_, __FILE__, __LINE__); \
    } while (0);

#endif // defined(NDEBUG)

//------------------------------------------------------------------------------
// Types

//--------------------------------------
// Enum of vertex attributes
//--------------------------------------
typedef enum attribute
{
    ATTRIB_POSITION,
    ATTRIB_COLOR,
    ATTRIB_TEXCOORD,
} gl_vertex_attrib_t;

//--------------------------------------
// Vertex attribute bits
//--------------------------------------
enum
{
    ATTRIB_BIT_POSITION = (1 << ATTRIB_POSITION),
    ATTRIB_BIT_COLOR = (1 << ATTRIB_COLOR),
    ATTRIB_BIT_TEXCOORD = (1 << ATTRIB_TEXCOORD)
};

//--------------------------------------
// Enum of vertex shaders
//--------------------------------------
typedef enum gl_vertex_shader
{
    VERTEX_SHADER_COMMON,
    TOTAL_VERTEX_SHADERS,
} gl_vertex_shader_t;

//--------------------------------------
// Enum of fragment shaders
//--------------------------------------
typedef enum gl_fragment_shader
{
    FRAGMENT_SHADER_SOLID,
    FRAGMENT_SHADER_TEXTURED,
    TOTAL_FRAGMENT_SHADERS,
} gl_fragment_shader_t;

//--------------------------------------
// Enum of shaders
//--------------------------------------
typedef enum gl_shader
{
    SHADER_NONE = -1,
    SHADER_POINT,
    SHADER_LINE,
    SHADER_OUTLINE,
    SHADER_FILL,
    SHADER_TEXTURE,
    TOTAL_SHADERS,
} gl_shader_t;

//--------------------------------------
// Enum of uniforms
//--------------------------------------
typedef enum uniform
{
    UNIFORM_PROJECTION,
    UNIFORM_MODELVIEW,
    UNIFORM_COLOR,
    UNIFORM_TEXTURE,
    UNIFORM_TEXSIZE,
    TOTAL_UNIFORMS,
} gl_uniform_t;

//--------------------------------------
// Uniform bits
//--------------------------------------
enum
{
    UNIFORM_BIT_PROJECTION = (1 << UNIFORM_PROJECTION),
    UNIFORM_BIT_MODELVIEW = (1 << UNIFORM_MODELVIEW),
    UNIFORM_BIT_COLOR = (1 << UNIFORM_COLOR),
    UNIFORM_BIT_TEXTURE = (1 << UNIFORM_TEXTURE),
    UNIFORM_BIT_TEXSIZE = (1 << UNIFORM_TEXSIZE),
};

//--------------------------------------
// Vertex shader source code
//--------------------------------------
static char const *vertex_shader_source[TOTAL_VERTEX_SHADERS] = {
    [VERTEX_SHADER_COMMON] =
        "attribute vec2 a_position;\n"
        "attribute vec4 a_color;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec4 v_color;\n"
        "varying vec2 v_texCoord;\n"
        "uniform mat4 u_projection;\n"
        "uniform mat4 u_modelView;\n"
        "void main() {\n"
        "    v_texCoord = a_texCoord;\n"
        "    v_color = a_color;\n"
        "    vec4 position = vec4(a_position, 0.0, 1.0);\n"
        "    gl_Position = u_projection * u_modelView * position;\n"
        "}\n",
};

//--------------------------------------
// Fragment shader source code
//--------------------------------------
static char const *fragment_shader_source[TOTAL_FRAGMENT_SHADERS] = {
    [FRAGMENT_SHADER_SOLID] =
        "uniform vec4 u_color;\n"
        "void main() {\n"
        "    gl_FragColor = u_color;\n"
        "}\n",
    [FRAGMENT_SHADER_TEXTURED] =
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D u_texture;\n"
        "uniform vec2 u_texSize;\n"
        "void main() {\n"
        "    vec2 texCoord = v_texCoord / u_texSize;\n"
        "    gl_FragColor = texture2D(u_texture, texCoord);\n"
        "}\n",
};

//------------------------------------------------------------------------------

//--------------------------------------
// Structure to hold OpenGL texture metadata
//--------------------------------------
typedef struct gl_texture
{
    GLuint  id;
    GLuint  width;
    GLuint  height;
} gl_texture_t;

//--------------------------------------
// Main structure
//--------------------------------------
typedef struct gl_renderer_priv
{
    uint32_t            attrib_bits;
    gl_shader_t         shader_id;

    float               projection[16];
    float               model_view[16];
    GLclampf            color[TOTAL_SHADERS][4];
    int32_t             texture_id[TOTAL_SHADERS];

    gl_texture_t        *textures[TQ_TEXTURE_LIMIT];
    int32_t             current_texture_index;

    GLuint              vertex_shaders[TOTAL_VERTEX_SHADERS];
    GLuint              fragment_shaders[TOTAL_FRAGMENT_SHADERS];

    GLuint              shaders[TOTAL_SHADERS];
    GLint               uniform_locations[TOTAL_SHADERS][TOTAL_UNIFORMS];
    uint32_t            dirty_bits[TOTAL_SHADERS];
} gl_renderer_priv_t;

//------------------------------------------------------------------------------
// Module instance

static gl_renderer_priv_t gl;

//------------------------------------------------------------------------------
// Utility functions

//--------------------------------------
// Convert 24-bit tq_color_t to OpenGL color value.
//--------------------------------------
static void decode_rgb(GLclampf *dst, tq_color_t src)
{
    dst[0] = ((src >> 24) & 255) / 255.0f;
    dst[1] = ((src >> 16) & 255) / 255.0f;
    dst[2] = ((src >> 8) & 255) / 255.0f;
}

//--------------------------------------
// Convert 32-bit tq_color_t to OpenGL color value.
//--------------------------------------
static void decode_rgba(GLclampf *dst, tq_color_t src)
{
    dst[0] = ((src >> 24) & 255) / 255.0f;
    dst[1] = ((src >> 16) & 255) / 255.0f;
    dst[2] = ((src >> 8) & 255) / 255.0f;
    dst[3] = ((src >> 0) & 255) / 255.0f;
}

//--------------------------------------
// Compile a shader unit.
//--------------------------------------
static GLuint compile_shader(GLenum type, char const *source)
{
    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        GLchar buffer[1024];
        glGetShaderInfoLog(handle, sizeof(buffer), NULL, buffer);

        char const *type_str;

        if (type == GL_VERTEX_SHADER) {
            type_str = "vertex";
        } else if (type == GL_FRAGMENT_SHADER) {
            type_str = "fragment";
        } else {
            type_str = "unknown";
        }

        log_error("** Failed to link GLSL %s shader. **\n", type_str);
        log_error("%s\n", buffer);

        return 0;
    }

    return handle;
}

//--------------------------------------
// Build a shader program.
//--------------------------------------
static GLuint make_shader(gl_vertex_shader_t vs, gl_fragment_shader_t fs)
{
    if (gl.vertex_shaders[vs] == 0) {
        gl.vertex_shaders[vs] = compile_shader(GL_VERTEX_SHADER, vertex_shader_source[vs]);
    }

    if (gl.fragment_shaders[fs] == 0) {
        gl.fragment_shaders[fs] = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source[fs]);
    }

    GLuint handle = glCreateProgram();

    glAttachShader(handle, gl.vertex_shaders[vs]);
    glAttachShader(handle, gl.fragment_shaders[fs]);

    glBindAttribLocation(handle, ATTRIB_POSITION, "a_position");
    glBindAttribLocation(handle, ATTRIB_COLOR, "a_color");
    glBindAttribLocation(handle, ATTRIB_TEXCOORD, "a_texCoord");

    glLinkProgram(handle);

    GLint status;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);

    if (status != GL_TRUE) {
        GLchar buffer[1024];
        glGetProgramInfoLog(handle, sizeof(buffer), NULL, buffer);

        log_error("** Failed to link GLSL program. **\n");
        log_error("%s\n", buffer);

        return 0;
    }

    return handle;
}

//--------------------------------------
// Update current vertex format.
// Doesn't do anything if the format is the same.
//--------------------------------------
static void refresh_attrib_bits(int attrib_bits)
{
    if (gl.attrib_bits == attrib_bits) {
        return;
    }

    if (attrib_bits & ATTRIB_BIT_POSITION) {
        CHECK_GL(glEnableVertexAttribArray(ATTRIB_POSITION));
    } else {
        CHECK_GL(glDisableVertexAttribArray(ATTRIB_POSITION));
    }

    if (attrib_bits & ATTRIB_BIT_COLOR) {
        CHECK_GL(glEnableVertexAttribArray(ATTRIB_COLOR));
    } else {
        CHECK_GL(glDisableVertexAttribArray(ATTRIB_COLOR));
    }

    if (attrib_bits & ATTRIB_BIT_TEXCOORD) {
        CHECK_GL(glEnableVertexAttribArray(ATTRIB_TEXCOORD));
    } else {
        CHECK_GL(glDisableVertexAttribArray(ATTRIB_TEXCOORD));
    }

    gl.attrib_bits = attrib_bits;
}

//--------------------------------------
// Actually update color for the current shader.
//--------------------------------------
static void apply_color(void)
{
    int32_t shader_id = gl.shader_id;
    GLint location = gl.uniform_locations[shader_id][UNIFORM_COLOR];
    CHECK_GL(glUniform4fv(location, 1, gl.color[shader_id]));
}

//--------------------------------------
// Actually update texture for the current shader.
//--------------------------------------
static void apply_texture(void)
{
    int32_t shader_id = gl.shader_id;
    int32_t texture_id = gl.texture_id[shader_id];

    CHECK_GL(glActiveTexture(GL_TEXTURE0));

    if (texture_id == -1) {
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
    } else {
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, gl.textures[texture_id]->id));
        
        GLint u_texsize = gl.uniform_locations[shader_id][UNIFORM_TEXSIZE];
        CHECK_GL(glUniform2f(u_texsize,
            (GLfloat) gl.textures[texture_id]->width,
            (GLfloat) gl.textures[texture_id]->height));
    }

    GLint u_unit = gl.uniform_locations[shader_id][UNIFORM_TEXTURE];
    CHECK_GL(glUniform1i(u_unit, 0));
}

//--------------------------------------
// Update current shader.
// Also updates uniform values if there are changed ones.
// Doesn't do anything if the shader is the same.
//--------------------------------------
static void refresh_shader(gl_shader_t shader_id)
{
    if (gl.shader_id == shader_id) {
        return;
    }

    CHECK_GL(glUseProgram(gl.shaders[shader_id]));
    gl.shader_id = shader_id;

    if (gl.dirty_bits[shader_id] & UNIFORM_BIT_PROJECTION) {
        GLint location = gl.uniform_locations[shader_id][UNIFORM_PROJECTION];
        CHECK_GL(glUniformMatrix4fv(location, 1, GL_TRUE, gl.projection));
    }

    if (gl.dirty_bits[shader_id] & UNIFORM_BIT_MODELVIEW) {
        GLint location = gl.uniform_locations[shader_id][UNIFORM_MODELVIEW];
        CHECK_GL(glUniformMatrix4fv(location, 1, GL_TRUE, gl.model_view));
    }

    if (gl.dirty_bits[shader_id] & UNIFORM_BIT_COLOR) {
        apply_color();
    }

    if (gl.dirty_bits[shader_id] & UNIFORM_BIT_TEXTURE) {
        apply_texture();
    }

    gl.dirty_bits[shader_id] = 0;
}

//--------------------------------------
// Update current color value for the shader.
//--------------------------------------
static void refresh_color(gl_shader_t shader_id, tq_color_t color)
{
    decode_rgba(gl.color[shader_id], color);

    if (gl.shader_id == shader_id) {
        apply_color();
    } else {
        gl.dirty_bits[shader_id] |= UNIFORM_BIT_COLOR;
    }
}

//--------------------------------------
// Switch current texture for the given shader.
//--------------------------------------
static void refresh_texture(gl_shader_t shader_id, int32_t texture_id)
{
    // if (texture_id == gl.texture_id[shader_id]) {
    //     return;
    // }

    gl.texture_id[shader_id] = texture_id;

    if (gl.shader_id == shader_id) {
        apply_texture();
    } else {
        gl.dirty_bits[shader_id] |= UNIFORM_BIT_TEXTURE;
    }
}

//--------------------------------------
// Get unused identifier for a texture object.
//--------------------------------------
static int32_t get_texture_index()
{
    int32_t index = gl.current_texture_index;
        
    if (!gl.textures[index]) {
        gl.current_texture_index = (gl.current_texture_index + 1) % TQ_TEXTURE_LIMIT;
        return index;
    }

    for (int32_t id = 0; id < TQ_TEXTURE_LIMIT; id++) {
        if (gl.textures[index]) {
            continue;
        }

        gl.current_texture_index = (id + 1) % TQ_TEXTURE_LIMIT;
        return id;
    }

    return -1;
}

//------------------------------------------------------------------------------
// Module implementation

//--------------------------------------
// Initialize OpenGL renderer.
//--------------------------------------
static void initialize(void)
{
#if !defined(TQ_USE_OPENGL_ES)
    // Load OpenGL extensions.
    if (glewInit() != GLEW_OK) {
        log_error("Failed to initialize GLEW.\n");
    }
#endif

    // Set default values.
    // This is done per field on purpose.

    gl.attrib_bits = 0;
    gl.shader_id = SHADER_NONE;

    mat4_identity(gl.projection);
    mat4_identity(gl.model_view);
    memset(gl.color, 255, sizeof(gl.color));
    memset(gl.texture_id, 255, sizeof(gl.texture_id));

    memset(gl.textures, 0, sizeof(gl.textures));
    gl.current_texture_index = 0;

    memset(gl.vertex_shaders, 0, sizeof(gl.vertex_shaders));
    memset(gl.fragment_shaders, 0, sizeof(gl.fragment_shaders));

    memset(gl.shaders, 0, sizeof(gl.shaders));
    memset(gl.uniform_locations, 0, sizeof(gl.uniform_locations));
    memset(gl.dirty_bits, 255, sizeof(gl.dirty_bits));

    // Compile and link shader programs.
    // There is a single shader program for each "brush".

    int map[TOTAL_SHADERS][2] = {
        [SHADER_POINT]   = { VERTEX_SHADER_COMMON, FRAGMENT_SHADER_SOLID },
        [SHADER_LINE]    = { VERTEX_SHADER_COMMON, FRAGMENT_SHADER_SOLID },
        [SHADER_OUTLINE] = { VERTEX_SHADER_COMMON, FRAGMENT_SHADER_SOLID },
        [SHADER_FILL]    = { VERTEX_SHADER_COMMON, FRAGMENT_SHADER_SOLID },
        [SHADER_TEXTURE] = { VERTEX_SHADER_COMMON, FRAGMENT_SHADER_TEXTURED },
    };

    for (int shader_id = 0; shader_id < TOTAL_SHADERS; shader_id++) {
        gl_vertex_shader_t vs = map[shader_id][0];
        gl_fragment_shader_t fs = map[shader_id][1];

        gl.shaders[shader_id] = make_shader(vs, fs);

        char const *names[TOTAL_UNIFORMS] = {
            [UNIFORM_PROJECTION]    = "u_projection",
            [UNIFORM_MODELVIEW]     = "u_modelView",
            [UNIFORM_COLOR]         = "u_color",
            [UNIFORM_TEXTURE]       = "u_texture",
            [UNIFORM_TEXSIZE]       = "u_texSize",
        };

        for (int uniform = 0; uniform < TOTAL_UNIFORMS; uniform++) {
            gl.uniform_locations[shader_id][uniform]
                = glGetUniformLocation(gl.shaders[shader_id], names[uniform]);
        }
    }

    // Shaders units are not needed by now.

    for (int vs = 0; vs < TOTAL_VERTEX_SHADERS; vs++) {
        CHECK_GL(glDeleteShader(gl.vertex_shaders[vs]));
        gl.vertex_shaders[vs] = 0;
    }

    for (int fs = 0; fs < TOTAL_FRAGMENT_SHADERS; fs++) {
        CHECK_GL(glDeleteShader(gl.fragment_shaders[fs]));
        gl.fragment_shaders[fs] = 0;
    }

    // Reset OpenGL state.
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

#if !defined(TQ_USE_OPENGL_ES)
    glEnable(GL_MULTISAMPLE);
#endif

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialization is done.
    log_info("OpenGL renderer is initialized.\n");
    log_info("[  GL_VENDOR] %s\n", glGetString(GL_VENDOR));
    log_info("[GL_RENDERER] %s\n", glGetString(GL_RENDERER));
    log_info("[ GL_VERSION] %s\n", glGetString(GL_VERSION));
}

//--------------------------------------
// Terminate OpenGL renderer.
//--------------------------------------
static void terminate(void)
{
    // Delete all shader programs.
    for (int shader = 0; shader < TOTAL_SHADERS; shader++) {
        CHECK_GL(glDeleteProgram(gl.shaders[shader]));
    }

    // Free all textures that are still in use.
    for (int id = 0; id < TQ_TEXTURE_LIMIT; id++) {
        if (gl.textures[id]) {
            CHECK_GL(glDeleteTextures(1, &gl.textures[id]->id));
            free(gl.textures[id]);
        }
    }
}

//--------------------------------------
// Clear background.
//--------------------------------------
static void clear(void)
{
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
}

//--------------------------------------
// Set background color.
//--------------------------------------
static void set_clear_color(tq_color_t clear_color)
{
    GLclampf value[3];
    decode_rgb(value, clear_color);
    CHECK_GL(glClearColor(value[0], value[1], value[2], 1.0f));
}

//--------------------------------------
// Update viewport.
//--------------------------------------
static void update_viewport(int x, int y, int w, int h)
{
    CHECK_GL(glViewport(x, y, w, h));
}

//--------------------------------------
// Update projection matrix.
//--------------------------------------
static void update_projection(float const *mat4)
{
    mat4_copy(gl.projection, mat4);

    for (int shader_id = 0; shader_id < TOTAL_SHADERS; shader_id++) {
        GLint location = gl.uniform_locations[shader_id][UNIFORM_PROJECTION];

        if (shader_id == gl.shader_id) {
            CHECK_GL(glUniformMatrix4fv(location, 1, GL_TRUE, gl.projection));
        } else {
            gl.dirty_bits[shader_id] |= (1 << UNIFORM_PROJECTION);
        }
    }
}

//--------------------------------------
// Update model-view matrix.
//--------------------------------------
static void update_model_view(float const *mat3)
{
    // (Note: the "view" matrix corresponds to the projection matrix,
    //  and the "transform" one roughly corresponds to the model-view matrix
    //  of classic OpenGL).
    // (Note 2: OpenGL's own matrices are not used here, since I handle
    //  this in the "tq::graphics" module independently of renderer).

    mat4_expand(gl.model_view, mat3);

    for (int shader_id = 0; shader_id < TOTAL_SHADERS; shader_id++) {
        GLint location = gl.uniform_locations[shader_id][UNIFORM_MODELVIEW];

        if (shader_id == gl.shader_id) {
            CHECK_GL(glUniformMatrix4fv(location, 1, GL_TRUE, gl.model_view));
        } else {
            gl.dirty_bits[shader_id] |= UNIFORM_BIT_MODELVIEW;
        }
    }
}

//--------------------------------------
// Draw array of vertices as points.
//--------------------------------------
static void draw_points(float const *data, uint32_t num_vertices)
{
    refresh_attrib_bits(ATTRIB_BIT_POSITION);
    refresh_shader(SHADER_POINT);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, data));
    CHECK_GL(glDrawArrays(GL_POINTS, 0, num_vertices));
}

//--------------------------------------
// Draw array of vertices as lines.
//--------------------------------------
static void draw_lines(float const *data, uint32_t num_vertices)
{
    refresh_attrib_bits(ATTRIB_BIT_POSITION);
    refresh_shader(SHADER_LINE);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, data));
    CHECK_GL(glDrawArrays(GL_LINES, 0, num_vertices));
}

//--------------------------------------
// Draw array of vertices as a shape outline.
//--------------------------------------
static void draw_outline(float const *data, uint32_t num_vertices)
{
    refresh_attrib_bits(ATTRIB_BIT_POSITION);
    refresh_shader(SHADER_OUTLINE);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, data));
    CHECK_GL(glDrawArrays(GL_LINE_LOOP, 0, num_vertices));
}

//--------------------------------------
// Draw array of vertices as a shape fill.
//--------------------------------------
static void draw_fill(float const *data, uint32_t num_vertices)
{
    refresh_attrib_bits(ATTRIB_BIT_POSITION);
    refresh_shader(SHADER_FILL);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, data));
    CHECK_GL(glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices));
}

//--------------------------------------
// Set color of points.
//--------------------------------------
static void set_point_color(tq_color_t point_color)
{
    refresh_color(SHADER_POINT, point_color);
}

//--------------------------------------
// Set color of lines.
//--------------------------------------
static void set_line_color(tq_color_t line_color)
{
    refresh_color(SHADER_LINE, line_color);
}

//--------------------------------------
// Set color of outlines.
//--------------------------------------
static void set_outline_color(tq_color_t outline_color)
{
    refresh_color(SHADER_OUTLINE, outline_color);
}

//--------------------------------------
// Set fill color.
//--------------------------------------
static void set_fill_color(tq_color_t fill_color)
{
    refresh_color(SHADER_FILL, fill_color);
}

//--------------------------------------
// Load a texture from an abstract stream.
//--------------------------------------
static int32_t load_texture(stream_t const *stream)
{
    image_t image = { 0 };
    int32_t status = image_load(&image, stream);

    if (status < 0) {
        return TQ_INVALID_HANDLE;
    }

    GLenum format;

    switch (image.pixel_format) {
    case PIXEL_FORMAT_GRAYSCALE:
        format = GL_LUMINANCE;
        break;
    case PIXEL_FORMAT_GRAYSCALE_ALPHA:
        format = GL_LUMINANCE_ALPHA;
        break;
    case PIXEL_FORMAT_RGB:
        format = GL_RGB;
        break;
    case PIXEL_FORMAT_RGBA:
        format = GL_RGBA;
        break;
    default:
        format = GL_INVALID_ENUM;
        break;
    }

    // Current texture state is going to be altered.
    // We have to reset these values so on the next
    // attempt to draw something the state will be forcibly
    // refreshed and the correct texture will be used.
    gl.texture_id[gl.shader_id] = -1;
    gl.shader_id = -1;

    GLuint id;
    CHECK_GL(glGenTextures(1, &id));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, id));

    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, format,
        image.width, image.height, 0, format,
        GL_UNSIGNED_BYTE, image.pixels));

    int32_t index = get_texture_index();

    gl.textures[index] = malloc(sizeof(gl_texture_t));
    gl.textures[index]->id = id;
    gl.textures[index]->width = image.width;
    gl.textures[index]->height = image.height;

    image_free(&image);
    return index;
}

//--------------------------------------
// Delete a texture
//--------------------------------------
static void delete_texture(int32_t texture_id)
{
    CHECK_GL(glDeleteTextures(1, &gl.textures[texture_id]->id));
    free(gl.textures[texture_id]);
}

//--------------------------------------
// Get size of a texture
//--------------------------------------
static void get_texture_size(int32_t texture_id, uint32_t *width, uint32_t *height)
{
    *width = gl.textures[texture_id]->width;
    *height = gl.textures[texture_id]->height;
}

//--------------------------------------
// Draw textured rectangle
//--------------------------------------
static void draw_texture(int32_t texture_id, float const *data, uint32_t num_vertices)
{
    refresh_attrib_bits(ATTRIB_BIT_POSITION | ATTRIB_BIT_TEXCOORD);
    refresh_shader(SHADER_TEXTURE);
    refresh_texture(SHADER_TEXTURE, texture_id);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, data + 0));
    CHECK_GL(glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, data + 2));
    CHECK_GL(glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices));
}

//--------------------------------------
// Called at the end of a frame.
//--------------------------------------
static void flush(void)
{
    // Is it even needed?
    CHECK_GL(glFlush());
}

//------------------------------------------------------------------------------
// Module constructor

//--------------------------------------
// Construct abstract module for OpenGL renderer.
//--------------------------------------
void tq_construct_gl_renderer(tq_renderer_t *renderer)
{
    *renderer = (tq_renderer_t) {
        .initialize         = initialize,
        .terminate          = terminate,
        
        .clear              = clear,
        .set_clear_color    = set_clear_color,

        .update_viewport    = update_viewport,
        .update_projection  = update_projection,
        .update_model_view  = update_model_view,

        .draw_points        = draw_points,
        .draw_lines         = draw_lines,
        .draw_outline       = draw_outline,
        .draw_fill          = draw_fill,

        .set_point_color    = set_point_color,
        .set_line_color     = set_line_color,
        .set_outline_color  = set_outline_color,
        .set_fill_color     = set_fill_color,

        .load_texture       = load_texture,
        .delete_texture     = delete_texture,
        .get_texture_size   = get_texture_size,
        .draw_texture       = draw_texture,

        .flush              = flush,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_USE_OPENGL)

//------------------------------------------------------------------------------
