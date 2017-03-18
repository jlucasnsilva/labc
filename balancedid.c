/*
  License: zlib license

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/


#include "balancedid.h"


balancedgen_t balancedgen_make()
{
    balancedgen_t gen;
    balancedgen_reset(&gen);
    return gen;
}

balancedgen_t* balancedgen_new()
{
    balancedgen_t* gen = malloc( sizeof(balancedgen_t) );
    balancedgen_reset(gen);
    return gen;
}

uint32_t balancedgen_next(balancedgen_t *restrict g)
{
    uint32_t n  = 1 << g->_tree_level;
    uint32_t id = BALANCEDID_NULL_ID;

    // check if g != NULL and if the generator isn't
    // exhausted/done yet.
    if (g && !g->_done) {
        // if the last id of this tree has already been
        // generated, go to the next tree level.
        if (g->_index >= n) {
            g->_tree_level++;
            g->_index = 0;
            g->_seed /= 2;
        }

        id = g->_seed * (2 * g->_index + 1);
        ++g->_index;
    
        // if the generator has been exhausted, set the
        // generator as done.
        if (id == (MAX_BALANCEDID - 1)) {
            g->_done = true;
        }
    }

    return id;
}

void balancedgen_reset(balancedgen_t *restrict g)
{
    if (g) {
        g->_tree_level = 0;
        g->_index      = 0;
        g->_seed       = MAX_BALANCEDID / 2;
        g->_done       = false;
    }
}
