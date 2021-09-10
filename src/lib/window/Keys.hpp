/* KEYS.hpp
 *   by Lut99
 *
 * Created:
 *   16/07/2021, 14:47:41
 * Last edited:
 *   16/07/2021, 14:47:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains an enum that describes certain keys.
**/

#ifndef WINDOW_KEYS_HPP
#define WINDOW_KEYS_HPP

#include <cstdint>

namespace Makma3D {
    /* Data type to which the KeyboardKey enum is cast. */
    using keyboardkey_t = uint64_t;

    /* Enum used to describe the keys within GLFW. */
    enum class KeyboardKey {
        /* Value that indicates no key. */
        none = 0,
        /* Value that indicates all keys. */
        all = ~0,

        /* 0-key. */
        zero = 0x1,
        /* 1-key. */
        one = 0x2,
        /* 2-key. */
        two = 0x4,
        /* 3-key. */
        three = 0x8,
        /* 4-key. */
        four = 0x10,
        /* 5-key. */
        five = 0x20,
        /* 6-key. */
        six = 0x40,
        /* 7-key. */
        seven = 0x80,
        /* 8-key. */
        eight = 0x100,
        /* 9-key. */
        nine = 0x200,

        /* A-key. */
        a = 0x400,
        /* B-key. */
        b = 0x800,
        /* C-key. */
        c = 0x1000,
        /* D-key. */
        d = 0x2000,
        /* E-key. */
        e = 0x4000,
        /* F-key. */
        f = 0x8000,
        /* G-key. */
        g = 0x10000,
        /* H-key. */
        h = 0x20000,
        /* I-key. */
        i = 0x40000,
        /* J-key. */
        j = 0x80000,
        /* K-key. */
        k = 0x100000,
        /* L-key. */
        l = 0x200000,
        /* M-key. */
        m = 0x400000,
        /* N-key. */
        n = 0x800000,
        /* O-key. */
        o = 0x1000000,
        /* P-key. */
        p = 0x2000000,
        /* Q-key. */
        q = 0x4000000,
        /* R-key. */
        r = 0x8000000,
        /* S-key. */
        s = 0x10000000,
        /* T-key. */
        t = 0x20000000,
        /* U-key. */
        u = 0x1000000,
        /* V-key. */
        v = 0x2000000,
        /* W-key. */
        w = 0x4000000,
        /* X-key. */
        x = 0x8000000,
        /* Y-key. */
        y = 0x10000000,
        /* Z-key. */
        z = 0x20000000,
        
        /* Left control key. */
        lctrl = 0x40000000,
        /* Right control key. */
        rctrl = 0x80000000,
        /* Any control key. */
        ctrl = (keyboardkey_t) lctrl | (keyboardkey_t) rctrl,

        /* Left shift key. */
        lshift = 0x100000000,
        /* Right shift key. */
        rshift = 0x200000000,
        /* Any shift key. */
        shift = (keyboardkey_t) lshift | (keyboardkey_t) rshift,

        /* Left alt key. */
        lalt = 0x400000000,
        /* Right alt key. */
        ralt = 0x400000000,
        /* Any alt key. */
        alt = (keyboardkey_t) lalt | (keyboardkey_t) ralt

    };
    
}

#endif
