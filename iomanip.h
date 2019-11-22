/*
 * zero - pre-emptive multitasking kernel for AVR
 *
 *  Techno Cosmic Research Institute	Dirk Mahoney			dirk@tcri.com.au
 *  Catchpole Robotics					Christian Catchpole		christian@catchpole.net
 * 
 */

#ifndef TCRI_ZERO_IOMANIP_H
#define TCRI_ZERO_IOMANIP_H

#include <stdint.h>
#include "textpipe.h"

namespace zero {

    struct settextcolor {
        settextcolor(const Color color);
        Color _color;
    };

    #define black settextcolor(Color::BLACK)
    #define red settextcolor(Color::RED)
    #define green settextcolor(Color::GREEN)
    #define yellow settextcolor(Color::YELLOW)
    #define blue settextcolor(Color::BLUE)
    #define magenta settextcolor(Color::MAGENTA)
    #define cyan settextcolor(Color::CYAN)
    #define white settextcolor(Color::WHITE)

    struct setbackcolor {
        setbackcolor(const Color color);
        Color _color;
    };

    struct setbase {
        setbase(const uint8_t base);
        uint8_t _base;
    };

    #define bin setbase(2)
    #define oct setbase(8)
    #define dec setbase(10)
    #define hex setbase(16)

    struct setw {
        setw(const int8_t width);
        int8_t _width;
    };

    struct setfill {
        setfill(const char c);
        char _fill;
    };

    struct setalignment {
        setalignment(const Alignment alignment);
        Alignment _alignment;
    };

    #define left setalignment(Alignment::LEFT)
    #define right setalignment(Alignment::RIGHT)

    struct setuppercase {
        setuppercase(const bool v);
        bool _uppercase;
    };

    #define uppercase setuppercase(true)
    #define nouppercase setuppercase(false)

    struct setreverse {
        setreverse(const bool v);
        bool _reverse;
    };

    struct repeat {
        repeat(const char v, const uint16_t c);
        char _v;
        uint16_t _c;
    };

    #define endl PGM(__endl)

}

extern const char __endl[];

zero::TextPipe& operator<<(zero::TextPipe&, const zero::settextcolor);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::setbackcolor);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::setbase);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::setw);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::setfill);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::setalignment);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::setuppercase);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::setreverse);
zero::TextPipe& operator<<(zero::TextPipe&, const zero::repeat);

#endif
