
//------------------------------------------------------------------------------

#include <cstdint>
#include <cstring>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>

extern "C" {
    #include "tq_display.h"
    #include "tq_log.h"
    #include "tq_renderer.h"
}

//------------------------------------------------------------------------------

namespace tq
{
    namespace sf
    {
        extern ::sf::RenderTarget *canvas;
    }
}

//------------------------------------------------------------------------------

namespace
{
    sf::RenderTarget *canvas;

    sf::Color clear_color;
    sf::Color point_color;
    sf::Color line_color;
    sf::Color outline_color;
    sf::Color fill_color;

    sf::Texture *texture_array[TQ_TEXTURE_LIMIT];
    std::int32_t current_texture_index;

    sf::Transform transform_;

    std::int32_t get_texture_index()
    {
        std::int32_t index = current_texture_index;
        
        if (texture_array[index] == nullptr) {
            current_texture_index = (current_texture_index + 1) % TQ_TEXTURE_LIMIT;
            return index;
        }

        for (std::int32_t n = 0; n < TQ_TEXTURE_LIMIT; n++) {
            if (texture_array[index]) {
                continue;
            }

            current_texture_index = (n + 1) % TQ_TEXTURE_LIMIT;
            return n;
        }

        return -1;
    }

    void initialize()
    {
        canvas = tq::sf::canvas;
    }

    void terminate()
    {
        for (std::int32_t id = 0; id < TQ_TEXTURE_LIMIT; id++) {
            delete texture_array[id];
        }
    }

    void clear()
    {
        canvas->clear(clear_color);
    }

    void set_clear_color(tq_color_t clear_color)
    {
        ::clear_color = sf::Color(clear_color);
    }

    void set_view(float x, float y, float width, float height, float rotation)
    {
        sf::View view({ x, y }, { width, height });
        view.setRotation(rotation);

        canvas->setView(view);
    }

    void reset_view()
    {
        canvas->setView(canvas->getDefaultView());
    }

    void transform(float const *matrix)
    {
        transform_ = sf::Transform(
            matrix[0], matrix[1], matrix[2],
            matrix[3], matrix[4], matrix[5],
            matrix[6], matrix[7], matrix[8]
        );
    }

    void draw_points(float const *data, unsigned int length)
    {
        auto vertices = new sf::Vertex[length];

        for (unsigned int i = 0; i < length; i++) {
            vertices[i].position.x = data[2 * i + 0];
            vertices[i].position.y = data[2 * i + 1];
            vertices[i].color = point_color;
        }

        canvas->draw(vertices, length, sf::Points, sf::RenderStates(transform_));
        delete[] vertices;
    }

    void draw_lines(float const *data, unsigned int length)
    {
        auto vertices = new sf::Vertex[length];

        for (unsigned int i = 0; i < length; i++) {
            vertices[i].position.x = data[2 * i + 0];
            vertices[i].position.y = data[2 * i + 1];
            vertices[i].color = line_color;
        }

        canvas->draw(vertices, length, sf::Lines, sf::RenderStates(transform_));
        delete[] vertices;
    }

    void draw_outline(float const *data, unsigned int length)
    {
        auto vertices = new sf::Vertex[length + 1];

        for (unsigned int i = 0; i < length; i++) {
            vertices[i].position.x = data[2 * i + 0];
            vertices[i].position.y = data[2 * i + 1];
            vertices[i].color = outline_color;
        }

        vertices[length] = vertices[0];

        canvas->draw(vertices, length + 1, sf::LineStrip, sf::RenderStates(transform_));
        delete[] vertices;
    }

    void draw_fill(float const *data, unsigned int length)
    {
        auto vertices = new sf::Vertex[length];

        for (unsigned int i = 0; i < length; i++) {
            vertices[i].position.x = data[2 * i + 0];
            vertices[i].position.y = data[2 * i + 1];
            vertices[i].color = fill_color;
        }

        canvas->draw(vertices, length, sf::TriangleFan, sf::RenderStates(transform_));
        delete[] vertices;
    }

    void set_point_color(tq_color_t point_color)
    {
        ::point_color = sf::Color(point_color);
    }

    void set_line_color(tq_color_t line_color)
    {
        ::line_color = sf::Color(line_color);
    }

    void set_outline_color(tq_color_t outline_color)
    {
        ::outline_color = sf::Color(outline_color);
    }

    void set_fill_color(tq_color_t fill_color)
    {
        ::fill_color = sf::Color(fill_color);
    }

    tq_handle_t load_texture(std::uint8_t const *buffer, std::size_t length)
    {
        std::int32_t index = get_texture_index();

        if (index < 0) {
            return TQ_INVALID_HANDLE;
        }

        texture_array[index] = new sf::Texture();

        if (!texture_array[index]) {
            return TQ_INVALID_HANDLE;
        }
            
        if (!texture_array[index]->loadFromMemory(buffer, length)) {
            return TQ_INVALID_HANDLE;
        }

        return static_cast<tq_handle_t>(index);
    }

    void delete_texture(tq_handle_t texture_handle)
    {
        delete texture_array[texture_handle];
        texture_array[texture_handle] = nullptr;
    }

    void get_texture_size(tq_handle_t texture_handle, std::uint32_t *width, std::uint32_t *height)
    {
        *width = texture_array[texture_handle]->getSize().x;
        *height = texture_array[texture_handle]->getSize().y;
    }

    void draw_texture(float const *data, unsigned int length, tq_handle_t texture_handle)
    {
        auto vertices = new sf::Vertex[length];

        for (unsigned int i = 0; i < length; i++) {
            vertices[i].position.x = data[4 * i + 0];
            vertices[i].position.y = data[4 * i + 1];
            vertices[i].texCoords.x = data[4 * i + 2];
            vertices[i].texCoords.y = data[4 * i + 3];
            vertices[i].color = sf::Color::White;
        }

        canvas->draw(vertices, length, sf::TriangleFan, sf::RenderStates(
            sf::BlendAlpha,
            transform_,
            texture_array[texture_handle],
            nullptr
        ));

        delete[] vertices;
    }

    void flush()
    {
    }
}

//------------------------------------------------------------------------------

void construct_sf_renderer(struct renderer *renderer)
{
    renderer->initialize        = ::initialize;
    renderer->terminate         = ::terminate;
    
    renderer->clear             = ::clear;
    renderer->set_clear_color   = ::set_clear_color;

    renderer->set_view          = ::set_view;
    renderer->reset_view        = ::reset_view;
    renderer->transform         = ::transform;

    renderer->draw_points       = ::draw_points;
    renderer->draw_lines        = ::draw_lines;
    renderer->draw_outline      = ::draw_outline;
    renderer->draw_fill         = ::draw_fill;

    renderer->set_point_color   = ::set_point_color;
    renderer->set_line_color    = ::set_line_color;
    renderer->set_outline_color = ::set_outline_color;
    renderer->set_fill_color    = ::set_fill_color;

    renderer->load_texture      = ::load_texture;
    renderer->delete_texture    = ::delete_texture;
    renderer->get_texture_size  = ::get_texture_size;
    renderer->draw_texture      = ::draw_texture;

    renderer->flush             = ::flush;
}

//------------------------------------------------------------------------------
