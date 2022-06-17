
//------------------------------------------------------------------------------

#include <cstdint>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Window.hpp>

extern "C" {
    #include "tq_core.h"
    #include "tq_display.h"
    #include "tq_log.h"
    #include "tq_renderer.h"
}

//------------------------------------------------------------------------------

namespace tq
{
    namespace sf
    {
        ::sf::RenderTarget *canvas;
    }
}

//------------------------------------------------------------------------------

namespace
{
    void center_window(sf::Window *window)
    {
        auto window_size = window->getSize();
        auto desktop_mode = sf::VideoMode::getDesktopMode();

        window->setPosition({
            static_cast<int>((desktop_mode.width / 2) - (window_size.x / 2)),
            static_cast<int>((desktop_mode.height / 2) - (window_size.y / 2))
        });
    }

    tq_key_t key_conv(sf::Keyboard::Key key)
    {
        switch (key) {
            case sf::Keyboard::Num0:        return TQ_KEY_0;
            case sf::Keyboard::Num1:        return TQ_KEY_1;
            case sf::Keyboard::Num2:        return TQ_KEY_2;
            case sf::Keyboard::Num3:        return TQ_KEY_3;
            case sf::Keyboard::Num4:        return TQ_KEY_4;
            case sf::Keyboard::Num5:        return TQ_KEY_5;
            case sf::Keyboard::Num6:        return TQ_KEY_6;
            case sf::Keyboard::Num7:        return TQ_KEY_7;
            case sf::Keyboard::Num8:        return TQ_KEY_8;
            case sf::Keyboard::Num9:        return TQ_KEY_9;
            case sf::Keyboard::A:           return TQ_KEY_A;
            case sf::Keyboard::B:           return TQ_KEY_B;
            case sf::Keyboard::C:           return TQ_KEY_C;
            case sf::Keyboard::D:           return TQ_KEY_D;
            case sf::Keyboard::E:           return TQ_KEY_E;
            case sf::Keyboard::F:           return TQ_KEY_F;
            case sf::Keyboard::G:           return TQ_KEY_G;
            case sf::Keyboard::H:           return TQ_KEY_H;
            case sf::Keyboard::I:           return TQ_KEY_I;
            case sf::Keyboard::J:           return TQ_KEY_J;
            case sf::Keyboard::K:           return TQ_KEY_K;
            case sf::Keyboard::L:           return TQ_KEY_L;
            case sf::Keyboard::M:           return TQ_KEY_M;
            case sf::Keyboard::N:           return TQ_KEY_N;
            case sf::Keyboard::O:           return TQ_KEY_O;
            case sf::Keyboard::P:           return TQ_KEY_P;
            case sf::Keyboard::Q:           return TQ_KEY_Q;
            case sf::Keyboard::R:           return TQ_KEY_R;
            case sf::Keyboard::S:           return TQ_KEY_S;
            case sf::Keyboard::T:           return TQ_KEY_T;
            case sf::Keyboard::U:           return TQ_KEY_U;
            case sf::Keyboard::V:           return TQ_KEY_V;
            case sf::Keyboard::W:           return TQ_KEY_W;
            case sf::Keyboard::X:           return TQ_KEY_X;
            case sf::Keyboard::Y:           return TQ_KEY_Y;
            case sf::Keyboard::Z:           return TQ_KEY_Z;
            case sf::Keyboard::Tilde:       return TQ_KEY_GRAVE;
            case sf::Keyboard::Quote:       return TQ_KEY_APOSTROPHE;
            case sf::Keyboard::Hyphen:      return TQ_KEY_MINUS;
            case sf::Keyboard::Equal:       return TQ_KEY_EQUAL;
            case sf::Keyboard::LBracket:    return TQ_KEY_LBRACKET;
            case sf::Keyboard::RBracket:    return TQ_KEY_RBRACKET;
            case sf::Keyboard::Comma:       return TQ_KEY_COMMA;
            case sf::Keyboard::Period:      return TQ_KEY_PERIOD;
            case sf::Keyboard::Semicolon:   return TQ_KEY_SEMICOLON;
            case sf::Keyboard::Slash:       return TQ_KEY_SLASH;
            case sf::Keyboard::Backslash:   return TQ_KEY_BACKSLASH;
            case sf::Keyboard::Space:       return TQ_KEY_SPACE;
            case sf::Keyboard::Escape:      return TQ_KEY_ESCAPE;
            case sf::Keyboard::Backspace:   return TQ_KEY_BACKSPACE;
            case sf::Keyboard::Tab:         return TQ_KEY_TAB;
            case sf::Keyboard::Enter:       return TQ_KEY_ENTER;
            case sf::Keyboard::F1:          return TQ_KEY_F1;
            case sf::Keyboard::F2:          return TQ_KEY_F2;
            case sf::Keyboard::F3:          return TQ_KEY_F3;
            case sf::Keyboard::F4:          return TQ_KEY_F4;
            case sf::Keyboard::F5:          return TQ_KEY_F5;
            case sf::Keyboard::F6:          return TQ_KEY_F6;
            case sf::Keyboard::F7:          return TQ_KEY_F7;
            case sf::Keyboard::F8:          return TQ_KEY_F8;
            case sf::Keyboard::F9:          return TQ_KEY_F9;
            case sf::Keyboard::F10:         return TQ_KEY_F10;
            case sf::Keyboard::F11:         return TQ_KEY_F11;
            case sf::Keyboard::F12:         return TQ_KEY_F12;
            case sf::Keyboard::Up:          return TQ_KEY_UP;
            case sf::Keyboard::Down:        return TQ_KEY_DOWN;
            case sf::Keyboard::Left:        return TQ_KEY_LEFT;
            case sf::Keyboard::Right:       return TQ_KEY_RIGHT;
            case sf::Keyboard::LShift:      return TQ_KEY_LSHIFT;
            case sf::Keyboard::RShift:      return TQ_KEY_RSHIFT;
            case sf::Keyboard::LControl:    return TQ_KEY_LCTRL;
            case sf::Keyboard::RControl:    return TQ_KEY_RCTRL;
            case sf::Keyboard::LAlt:        return TQ_KEY_LALT;
            case sf::Keyboard::RAlt:        return TQ_KEY_RALT;
            case sf::Keyboard::LSystem:     return TQ_KEY_LSUPER;
            case sf::Keyboard::RSystem:     return TQ_KEY_RSUPER;
            case sf::Keyboard::Menu:        return TQ_KEY_MENU;
            case sf::Keyboard::PageUp:      return TQ_KEY_PGUP;
            case sf::Keyboard::PageDown:    return TQ_KEY_PGDN;
            case sf::Keyboard::Home:        return TQ_KEY_HOME;
            case sf::Keyboard::End:         return TQ_KEY_END;
            case sf::Keyboard::Insert:      return TQ_KEY_INSERT;
            case sf::Keyboard::Delete:      return TQ_KEY_DELETE;
            case sf::Keyboard::Pause:       return TQ_KEY_PAUSE;
            case sf::Keyboard::Numpad0:     return TQ_KEY_KP_0;
            case sf::Keyboard::Numpad1:     return TQ_KEY_KP_1;
            case sf::Keyboard::Numpad2:     return TQ_KEY_KP_2;
            case sf::Keyboard::Numpad3:     return TQ_KEY_KP_3;
            case sf::Keyboard::Numpad4:     return TQ_KEY_KP_4;
            case sf::Keyboard::Numpad5:     return TQ_KEY_KP_5;
            case sf::Keyboard::Numpad6:     return TQ_KEY_KP_6;
            case sf::Keyboard::Numpad7:     return TQ_KEY_KP_7;
            case sf::Keyboard::Numpad8:     return TQ_KEY_KP_8;
            case sf::Keyboard::Numpad9:     return TQ_KEY_KP_9;
            case sf::Keyboard::Multiply:    return TQ_KEY_KP_MUL;
            case sf::Keyboard::Add:         return TQ_KEY_KP_ADD;
            case sf::Keyboard::Subtract:    return TQ_KEY_KP_SUB;
            case sf::Keyboard::Divide:      return TQ_KEY_KP_DIV;
            default:                        return TQ_TOTAL_KEYS;
        }
    }

    tq_mouse_button_t mouse_button_conv(sf::Mouse::Button mouse_button)
    {
        switch (mouse_button) {
            case sf::Mouse::Left:           return TQ_MOUSE_BUTTON_LEFT;
            case sf::Mouse::Right:          return TQ_MOUSE_BUTTON_RIGHT;
            case sf::Mouse::Middle:         return TQ_MOUSE_BUTTON_MIDDLE;
            default:                        return TQ_TOTAL_MOUSE_BUTTONS;
        }
    }

    void on_key_pressed(sf::Event::KeyEvent const &event)
    {
        tq_key_t key = key_conv(event.code);

        if (key != TQ_TOTAL_KEYS) {
            core_on_key_pressed(key);
        }
    }

    void on_key_released(sf::Event::KeyEvent const &event)
    {
        tq_key_t key = key_conv(event.code);

        if (key != TQ_TOTAL_KEYS) {
            core_on_key_released(key);
        }
    }

    void on_mouse_button_pressed(sf::Event::MouseButtonEvent const &event)
    {
        tq_mouse_button_t mouse_button = mouse_button_conv(event.button);

        if (mouse_button != TQ_TOTAL_MOUSE_BUTTONS) {
            core_on_mouse_button_pressed(mouse_button);
        }
    }

    void on_mouse_button_released(sf::Event::MouseButtonEvent const &event)
    {
        tq_mouse_button_t mouse_button = mouse_button_conv(event.button);

        if (mouse_button != TQ_TOTAL_MOUSE_BUTTONS) {
            core_on_mouse_button_released(mouse_button);
        }
    }
}

namespace
{
    sf::Window *window;

    void initialize(std::uint32_t width, std::uint32_t height, char const *title)
    {
        auto render_window = new sf::RenderWindow(
            { width, height },
            title,
            sf::Style::Titlebar | sf::Style::Close
        );

        window = render_window;
        tq::sf::canvas = render_window;

        window->setKeyRepeatEnabled(false);
        window->setVerticalSyncEnabled(true);

        center_window(window);
    }

    void terminate()
    {
        delete window;
    }

    void present()
    {
        window->display();
    }

    bool process_events()
    {
        sf::Event event;

        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::KeyPressed:
                on_key_pressed(event.key);
                break;
            case sf::Event::KeyReleased:
                on_key_released(event.key);
                break;
            case sf::Event::MouseButtonPressed:
                on_mouse_button_pressed(event.mouseButton);
                break;
            case sf::Event::MouseButtonReleased:
                on_mouse_button_released(event.mouseButton);
                break;
            case sf::Event::MouseMoved:
                core_on_mouse_cursor_moved(event.mouseMove.x, event.mouseMove.y);
                break;
            case sf::Event::Closed:
                window->close();
                break;
            default:
                break;
            }
        }

        return window->isOpen();
    }

    void set_size(std::uint32_t width, std::uint32_t height)
    {
        window->setSize({ width, height });
    }

    void set_title(char const *title)
    {
        window->setTitle(title);
    }
}

//------------------------------------------------------------------------------

void construct_sf_display(struct display *display)
{
    display->initialize     = ::initialize;
    display->terminate      = ::terminate;
    display->present        = ::present;
    display->process_events = ::process_events;
    display->set_size       = ::set_size;
    display->set_title      = ::set_title;
}

//------------------------------------------------------------------------------
