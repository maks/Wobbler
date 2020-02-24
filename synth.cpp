#include <iostream>
#include "synth.h"
#include "defs.h"
#include "libs/maximilian.h"
    Synth::Synth() {
      std::cout << "init" << std::endl;
      carrier_freq = 440;
      modulator_freq = 1;
    }

    double Synth::tick() {
      return AMPLITUDE *
        carrier.sinewave(carrier_freq +
            (modulator.sinewave(modulator_freq)*100)
            );
    }

    void Synth::set_carrier_freq(float freq) {
      carrier_freq = freq;
    }

    void Synth::set_modulator_freq(float freq) {
      modulator_freq = freq;
    }
