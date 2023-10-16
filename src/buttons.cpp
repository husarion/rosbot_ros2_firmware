#include "buttons.hpp"

void init_button_and_attach_to_callbacks(mbed::InterruptIn *interrupt, void (*rise)(), void (*fall)()){
    interrupt->mode(PullUp);
    interrupt->fall(fall);
    interrupt->rise(rise);
}