//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute    Dirk Mahoney           dirk@tcri.com.au
// Catchpole Robotics                  Christian Catchpole    christian@catchpole.net
//


#ifndef TCRI_ZERO_DEBUG_H
#define TCRI_ZERO_DEBUG_H


#include <avr/pgmspace.h>


namespace zero {

    namespace debug {
        /// @private
        void init();

        void print( const char c );
        void print( const char* s, const bool fromFlash = false );
        void print( const uint16_t n, const int base = 10 );
        void assert( const bool v, const char* const msg, const int lineNumber = 0 );
    };

}    // namespace zero

#ifdef DEBUG_ENABLED
    #define dbg( x ) zero::debug::print( x )
    #define dbg_pgm( x ) zero::debug::print( (char*) ( PSTR( x ) ), true )
    #define dbg_int( x ) zero::debug::print( ( x ), 10 )

    #define dbg_assert( v, msg ) zero::debug::assert( ( v ), PSTR( msg ), __LINE__ );
#else
    #define dbg( x ) ;
    #define dbg_pgm( x ) ;
    #define dbg_int( x ) ;

    #define dbg_assert( v, msg ) ;
#endif


#endif
