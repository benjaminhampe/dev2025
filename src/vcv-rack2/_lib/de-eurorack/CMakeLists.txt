set(LIB_NAME de_eurorack)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/braids/analog_oscillator.h
        ${LIB_HEADER_DIR}/braids/digital_oscillator.h
        ${LIB_HEADER_DIR}/braids/envelope.h
        ${LIB_HEADER_DIR}/braids/excitation.h
        ${LIB_HEADER_DIR}/braids/macro_oscillator.h
        ${LIB_HEADER_DIR}/braids/parameter_interpolation.h
        ${LIB_HEADER_DIR}/braids/quantizer.h
        ${LIB_HEADER_DIR}/braids/quantizer_scales.h
        ${LIB_HEADER_DIR}/braids/resources.h
        ${LIB_HEADER_DIR}/braids/settings.h
        ${LIB_HEADER_DIR}/braids/signature_waveshaper.h
        ${LIB_HEADER_DIR}/braids/svf.h
        ${LIB_HEADER_DIR}/braids/vco_jitter_source.h
        # ${LIB_HEADER_DIR}/clouds
        # ${LIB_HEADER_DIR}/clouds/dsp
        ${LIB_HEADER_DIR}/clouds/dsp/audio_buffer.h
        ${LIB_HEADER_DIR}/clouds/dsp/correlator.h
        ${LIB_HEADER_DIR}/clouds/dsp/frame.h
        ${LIB_HEADER_DIR}/clouds/dsp/fx
        ${LIB_HEADER_DIR}/clouds/dsp/fx/diffuser.h
        ${LIB_HEADER_DIR}/clouds/dsp/fx/fx_engine.h
        ${LIB_HEADER_DIR}/clouds/dsp/fx/pitch_shifter.h
        ${LIB_HEADER_DIR}/clouds/dsp/fx/reverb.h
        ${LIB_HEADER_DIR}/clouds/dsp/grain.h
        ${LIB_HEADER_DIR}/clouds/dsp/granular_processor.h
        ${LIB_HEADER_DIR}/clouds/dsp/granular_sample_player.h
        ${LIB_HEADER_DIR}/clouds/dsp/looping_sample_player.h
        ${LIB_HEADER_DIR}/clouds/dsp/mu_law.h
        ${LIB_HEADER_DIR}/clouds/dsp/parameters.h
        # ${LIB_HEADER_DIR}/clouds/dsp/pvoc
        ${LIB_HEADER_DIR}/clouds/dsp/pvoc/frame_transformation.h
        ${LIB_HEADER_DIR}/clouds/dsp/pvoc/phase_vocoder.h
        ${LIB_HEADER_DIR}/clouds/dsp/pvoc/stft.h
        ${LIB_HEADER_DIR}/clouds/dsp/sample_rate_converter.h
        ${LIB_HEADER_DIR}/clouds/dsp/window.h
        ${LIB_HEADER_DIR}/clouds/dsp/wsola_sample_player.h
        ${LIB_HEADER_DIR}/clouds/resources.h
        # ${LIB_HEADER_DIR}/marbles
        ${LIB_HEADER_DIR}/marbles/note_filter.h
        # ${LIB_HEADER_DIR}/marbles/ramp
        ${LIB_HEADER_DIR}/marbles/ramp/ramp.h
        ${LIB_HEADER_DIR}/marbles/ramp/ramp_divider.h
        ${LIB_HEADER_DIR}/marbles/ramp/ramp_extractor.h
        ${LIB_HEADER_DIR}/marbles/ramp/ramp_generator.h
        ${LIB_HEADER_DIR}/marbles/ramp/slave_ramp.h
        # ${LIB_HEADER_DIR}/marbles/random
        ${LIB_HEADER_DIR}/marbles/random/distributions.h
        ${LIB_HEADER_DIR}/marbles/random/lag_processor.h
        ${LIB_HEADER_DIR}/marbles/random/output_channel.h
        ${LIB_HEADER_DIR}/marbles/random/quantizer.h
        ${LIB_HEADER_DIR}/marbles/random/random_generator.h
        ${LIB_HEADER_DIR}/marbles/random/random_sequence.h
        ${LIB_HEADER_DIR}/marbles/random/random_stream.h
        ${LIB_HEADER_DIR}/marbles/random/t_generator.h
        ${LIB_HEADER_DIR}/marbles/random/x_y_generator.h
        ${LIB_HEADER_DIR}/marbles/resources.h
        ${LIB_HEADER_DIR}/marbles/scale_recorder.h
        # ${LIB_HEADER_DIR}/peaks
        # ${LIB_HEADER_DIR}/peaks/drums
        ${LIB_HEADER_DIR}/peaks/drums/bass_drum.h
        ${LIB_HEADER_DIR}/peaks/drums/excitation.h
        ${LIB_HEADER_DIR}/peaks/drums/fm_drum.h
        ${LIB_HEADER_DIR}/peaks/drums/high_hat.h
        ${LIB_HEADER_DIR}/peaks/drums/snare_drum.h
        ${LIB_HEADER_DIR}/peaks/drums/svf.h
        ${LIB_HEADER_DIR}/peaks/gate_processor.h
        # ${LIB_HEADER_DIR}/peaks/modulations
        ${LIB_HEADER_DIR}/peaks/modulations/bouncing_ball.h
        ${LIB_HEADER_DIR}/peaks/modulations/lfo.h
        ${LIB_HEADER_DIR}/peaks/modulations/mini_sequencer.h
        ${LIB_HEADER_DIR}/peaks/modulations/multistage_envelope.h
        ${LIB_HEADER_DIR}/peaks/number_station
        ${LIB_HEADER_DIR}/peaks/number_station/number_station.h
        ${LIB_HEADER_DIR}/peaks/processors.h
        # ${LIB_HEADER_DIR}/peaks/pulse_processor
        ${LIB_HEADER_DIR}/peaks/pulse_processor/pulse_randomizer.h
        ${LIB_HEADER_DIR}/peaks/pulse_processor/pulse_shaper.h
        ${LIB_HEADER_DIR}/peaks/resources.h
        # ${LIB_HEADER_DIR}/plaits
        # ${LIB_HEADER_DIR}/plaits/dsp
        # ${LIB_HEADER_DIR}/plaits/dsp/chords
        ${LIB_HEADER_DIR}/plaits/dsp/chords/chord_bank.h
        # ${LIB_HEADER_DIR}/plaits/dsp/downsampler
        ${LIB_HEADER_DIR}/plaits/dsp/downsampler/4x_downsampler.h
        # ${LIB_HEADER_DIR}/plaits/dsp/drums
        ${LIB_HEADER_DIR}/plaits/dsp/drums/analog_bass_drum.h
        ${LIB_HEADER_DIR}/plaits/dsp/drums/analog_snare_drum.h
        ${LIB_HEADER_DIR}/plaits/dsp/drums/hi_hat.h
        ${LIB_HEADER_DIR}/plaits/dsp/drums/synthetic_bass_drum.h
        ${LIB_HEADER_DIR}/plaits/dsp/drums/synthetic_snare_drum.h
        ${LIB_HEADER_DIR}/plaits/dsp/dsp.h
        # ${LIB_HEADER_DIR}/plaits/dsp/engine
        ${LIB_HEADER_DIR}/plaits/dsp/engine/additive_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/bass_drum_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/chord_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/fm_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/grain_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/hi_hat_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/modal_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/noise_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/particle_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/snare_drum_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/speech_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/string_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/swarm_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/virtual_analog_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/waveshaping_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine/wavetable_engine.h
        # ${LIB_HEADER_DIR}/plaits/dsp/engine2
        ${LIB_HEADER_DIR}/plaits/dsp/engine2/arpeggiator.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine2/chiptune_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine2/phase_distortion_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine2/six_op_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine2/string_machine_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine2/virtual_analog_vcf_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/engine2/wave_terrain_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/envelope.h
        # ${LIB_HEADER_DIR}/plaits/dsp/fm
        ${LIB_HEADER_DIR}/plaits/dsp/fm/algorithms.h
        ${LIB_HEADER_DIR}/plaits/dsp/fm/dx_units.h
        ${LIB_HEADER_DIR}/plaits/dsp/fm/envelope.h
        ${LIB_HEADER_DIR}/plaits/dsp/fm/lfo.h
        ${LIB_HEADER_DIR}/plaits/dsp/fm/operator.h
        ${LIB_HEADER_DIR}/plaits/dsp/fm/patch.h
        ${LIB_HEADER_DIR}/plaits/dsp/fm/voice.h
        # ${LIB_HEADER_DIR}/plaits/dsp/fx
        ${LIB_HEADER_DIR}/plaits/dsp/fx/diffuser.h
        ${LIB_HEADER_DIR}/plaits/dsp/fx/ensemble.h
        ${LIB_HEADER_DIR}/plaits/dsp/fx/fx_engine.h
        ${LIB_HEADER_DIR}/plaits/dsp/fx/low_pass_gate.h
        ${LIB_HEADER_DIR}/plaits/dsp/fx/overdrive.h
        ${LIB_HEADER_DIR}/plaits/dsp/fx/sample_rate_reducer.h
        # ${LIB_HEADER_DIR}/plaits/dsp/noise
        # ${LIB_HEADER_DIR}/plaits/dsp/noise/clocked_noise.h
        ${LIB_HEADER_DIR}/plaits/dsp/noise/dust.h
        ${LIB_HEADER_DIR}/plaits/dsp/noise/fractal_random_generator.h
        ${LIB_HEADER_DIR}/plaits/dsp/noise/particle.h
        ${LIB_HEADER_DIR}/plaits/dsp/noise/smooth_random_generator.h
        # ${LIB_HEADER_DIR}/plaits/dsp/oscillator
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/formant_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/grainlet_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/harmonic_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/nes_triangle_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/sine_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/string_synth_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/super_square_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/variable_saw_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/variable_shape_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/vosim_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/wavetable_oscillator.h
        ${LIB_HEADER_DIR}/plaits/dsp/oscillator/z_oscillator.h
        # ${LIB_HEADER_DIR}/plaits/dsp/physical_modelling
        ${LIB_HEADER_DIR}/plaits/dsp/physical_modelling/delay_line.h
        ${LIB_HEADER_DIR}/plaits/dsp/physical_modelling/modal_voice.h
        ${LIB_HEADER_DIR}/plaits/dsp/physical_modelling/resonator.h
        ${LIB_HEADER_DIR}/plaits/dsp/physical_modelling/string.h
        ${LIB_HEADER_DIR}/plaits/dsp/physical_modelling/string_voice.h
        # ${LIB_HEADER_DIR}/plaits/dsp/speech
        ${LIB_HEADER_DIR}/plaits/dsp/speech/lpc_speech_synth.h
        ${LIB_HEADER_DIR}/plaits/dsp/speech/lpc_speech_synth_controller.h
        ${LIB_HEADER_DIR}/plaits/dsp/speech/lpc_speech_synth_words.h
        ${LIB_HEADER_DIR}/plaits/dsp/speech/naive_speech_synth.h
        ${LIB_HEADER_DIR}/plaits/dsp/speech/sam_speech_synth.h
        ${LIB_HEADER_DIR}/plaits/dsp/voice.h
        ${LIB_HEADER_DIR}/plaits/resources.h
        ${LIB_HEADER_DIR}/plaits/user_data.h
        # ${LIB_HEADER_DIR}/rings
        # ${LIB_HEADER_DIR}/rings/dsp
        ${LIB_HEADER_DIR}/rings/dsp/dsp.h
        ${LIB_HEADER_DIR}/rings/dsp/fm_voice.h
        ${LIB_HEADER_DIR}/rings/dsp/follower.h
        # ${LIB_HEADER_DIR}/rings/dsp/fx
        ${LIB_HEADER_DIR}/rings/dsp/fx/chorus.h
        ${LIB_HEADER_DIR}/rings/dsp/fx/ensemble.h
        ${LIB_HEADER_DIR}/rings/dsp/fx/fx_engine.h
        ${LIB_HEADER_DIR}/rings/dsp/fx/reverb.h
        ${LIB_HEADER_DIR}/rings/dsp/limiter.h
        ${LIB_HEADER_DIR}/rings/dsp/note_filter.h
        ${LIB_HEADER_DIR}/rings/dsp/onset_detector.h
        ${LIB_HEADER_DIR}/rings/dsp/part.h
        ${LIB_HEADER_DIR}/rings/dsp/patch.h
        ${LIB_HEADER_DIR}/rings/dsp/performance_state.h
        ${LIB_HEADER_DIR}/rings/dsp/plucker.h
        ${LIB_HEADER_DIR}/rings/dsp/resonator.h
        ${LIB_HEADER_DIR}/rings/dsp/string.h
        ${LIB_HEADER_DIR}/rings/dsp/string_synth_envelope.h
        ${LIB_HEADER_DIR}/rings/dsp/string_synth_oscillator.h
        ${LIB_HEADER_DIR}/rings/dsp/string_synth_part.h
        ${LIB_HEADER_DIR}/rings/dsp/string_synth_voice.h
        ${LIB_HEADER_DIR}/rings/dsp/strummer.h
        ${LIB_HEADER_DIR}/rings/resources.h
        # ${LIB_HEADER_DIR}/tides
        ${LIB_HEADER_DIR}/tides/generator.h
        ${LIB_HEADER_DIR}/tides/plotter.h
        ${LIB_HEADER_DIR}/tides/resources.h
        # ${LIB_HEADER_DIR}/tides2
        ${LIB_HEADER_DIR}/tides2/poly_slope_generator.h
        # ${LIB_HEADER_DIR}/tides2/ramp
        ${LIB_HEADER_DIR}/tides2/ramp/ramp_extractor.h
        ${LIB_HEADER_DIR}/tides2/ramp/ratio.h
        ${LIB_HEADER_DIR}/tides2/ramp_generator.h
        ${LIB_HEADER_DIR}/tides2/ramp_shaper.h
        ${LIB_HEADER_DIR}/tides2/resources.h
        # ${LIB_HEADER_DIR}/warps
        # ${LIB_HEADER_DIR}/warps/dsp
        ${LIB_HEADER_DIR}/warps/dsp/filter_bank.h
        ${LIB_HEADER_DIR}/warps/dsp/limiter.h
        ${LIB_HEADER_DIR}/warps/dsp/modulator.h
        ${LIB_HEADER_DIR}/warps/dsp/oscillator.h
        ${LIB_HEADER_DIR}/warps/dsp/parameters.h
        ${LIB_HEADER_DIR}/warps/dsp/quadrature_oscillator.h
        ${LIB_HEADER_DIR}/warps/dsp/quadrature_transform.h
        ${LIB_HEADER_DIR}/warps/dsp/sample_rate_conversion_filters.h
        ${LIB_HEADER_DIR}/warps/dsp/sample_rate_converter.h
        ${LIB_HEADER_DIR}/warps/dsp/vocoder.h
        ${LIB_HEADER_DIR}/warps/resources.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/braids/analog_oscillator.cc
        ${LIB_SOURCE_DIR}/braids/digital_oscillator.cc
        ${LIB_SOURCE_DIR}/braids/macro_oscillator.cc
        ${LIB_SOURCE_DIR}/braids/quantizer.cc
        ${LIB_SOURCE_DIR}/braids/resources.cc
        # ${LIB_SOURCE_DIR}/clouds
        # ${LIB_SOURCE_DIR}/clouds/dsp
        ${LIB_SOURCE_DIR}/clouds/dsp/correlator.cc
        ${LIB_SOURCE_DIR}/clouds/dsp/granular_processor.cc
        ${LIB_SOURCE_DIR}/clouds/dsp/mu_law.cc
        # ${LIB_SOURCE_DIR}/clouds/dsp/pvoc
        ${LIB_SOURCE_DIR}/clouds/dsp/pvoc/frame_transformation.cc
        ${LIB_SOURCE_DIR}/clouds/dsp/pvoc/phase_vocoder.cc
        ${LIB_SOURCE_DIR}/clouds/dsp/pvoc/stft.cc
        ${LIB_SOURCE_DIR}/clouds/resources.cc
        # ${LIB_SOURCE_DIR}/marbles
        # ${LIB_SOURCE_DIR}/marbles/ramp
        ${LIB_SOURCE_DIR}/marbles/ramp/ramp_extractor.cc
        # ${LIB_SOURCE_DIR}/marbles/random
        ${LIB_SOURCE_DIR}/marbles/random/lag_processor.cc
        ${LIB_SOURCE_DIR}/marbles/random/output_channel.cc
        ${LIB_SOURCE_DIR}/marbles/random/quantizer.cc
        ${LIB_SOURCE_DIR}/marbles/random/t_generator.cc
        ${LIB_SOURCE_DIR}/marbles/random/x_y_generator.cc
        ${LIB_SOURCE_DIR}/marbles/resources.cc
        # ${LIB_SOURCE_DIR}/peaks
        # ${LIB_SOURCE_DIR}/peaks/drums
        ${LIB_SOURCE_DIR}/peaks/drums/bass_drum.cc
        ${LIB_SOURCE_DIR}/peaks/drums/fm_drum.cc
        ${LIB_SOURCE_DIR}/peaks/drums/high_hat.cc
        ${LIB_SOURCE_DIR}/peaks/drums/snare_drum.cc
        # ${LIB_SOURCE_DIR}/peaks/modulations
        ${LIB_SOURCE_DIR}/peaks/modulations/lfo.cc
        ${LIB_SOURCE_DIR}/peaks/modulations/multistage_envelope.cc
        # ${LIB_SOURCE_DIR}/peaks/number_station
        ${LIB_SOURCE_DIR}/peaks/number_station/number_station.cc
        ${LIB_SOURCE_DIR}/peaks/processors.cc
        # ${LIB_SOURCE_DIR}/peaks/pulse_processor
        ${LIB_SOURCE_DIR}/peaks/pulse_processor/pulse_randomizer.cc
        ${LIB_SOURCE_DIR}/peaks/pulse_processor/pulse_shaper.cc
        ${LIB_SOURCE_DIR}/peaks/resources.cc
        # ${LIB_SOURCE_DIR}/plaits
        # ${LIB_SOURCE_DIR}/plaits/dsp
        # ${LIB_SOURCE_DIR}/plaits/dsp/chords
        ${LIB_SOURCE_DIR}/plaits/dsp/chords/chord_bank.cc
        # ${LIB_SOURCE_DIR}/plaits/dsp/engine
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/additive_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/bass_drum_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/chord_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/fm_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/grain_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/hi_hat_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/modal_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/noise_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/particle_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/snare_drum_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/speech_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/string_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/swarm_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/virtual_analog_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/waveshaping_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine/wavetable_engine.cc
        # ${LIB_SOURCE_DIR}/plaits/dsp/engine2
        ${LIB_SOURCE_DIR}/plaits/dsp/engine2/chiptune_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine2/phase_distortion_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine2/six_op_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine2/string_machine_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine2/virtual_analog_vcf_engine.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/engine2/wave_terrain_engine.cc
        # ${LIB_SOURCE_DIR}/plaits/dsp/fm
        ${LIB_SOURCE_DIR}/plaits/dsp/fm/algorithms.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/fm/dx_units.cc
        # ${LIB_SOURCE_DIR}/plaits/dsp/physical_modelling
        ${LIB_SOURCE_DIR}/plaits/dsp/physical_modelling/modal_voice.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/physical_modelling/resonator.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/physical_modelling/string.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/physical_modelling/string_voice.cc
        # ${LIB_SOURCE_DIR}/plaits/dsp/speech
        ${LIB_SOURCE_DIR}/plaits/dsp/speech/lpc_speech_synth.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/speech/lpc_speech_synth_controller.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/speech/lpc_speech_synth_phonemes.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/speech/lpc_speech_synth_words.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/speech/naive_speech_synth.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/speech/sam_speech_synth.cc
        ${LIB_SOURCE_DIR}/plaits/dsp/voice.cc
        ${LIB_SOURCE_DIR}/plaits/resources.cc
        # ${LIB_SOURCE_DIR}/rings
        # ${LIB_SOURCE_DIR}/rings/dsp
        ${LIB_SOURCE_DIR}/rings/dsp/fm_voice.cc
        ${LIB_SOURCE_DIR}/rings/dsp/part.cc
        ${LIB_SOURCE_DIR}/rings/dsp/resonator.cc
        ${LIB_SOURCE_DIR}/rings/dsp/string.cc
        ${LIB_SOURCE_DIR}/rings/dsp/string_synth_part.cc
        ${LIB_SOURCE_DIR}/rings/resources.cc
        # ${LIB_SOURCE_DIR}/tides
        ${LIB_SOURCE_DIR}/tides/generator.cc
        ${LIB_SOURCE_DIR}/tides/plotter.cc
        ${LIB_SOURCE_DIR}/tides/resources.cc
        # ${LIB_SOURCE_DIR}/tides2
        ${LIB_SOURCE_DIR}/tides2/poly_slope_generator.cc
        ${LIB_SOURCE_DIR}/tides2/ramp
        ${LIB_SOURCE_DIR}/tides2/ramp/ramp_extractor.cc
        ${LIB_SOURCE_DIR}/tides2/resources.cc
        # ${LIB_SOURCE_DIR}/warps
        # ${LIB_SOURCE_DIR}/warps/dsp
        ${LIB_SOURCE_DIR}/warps/dsp/filter_bank.cc
        ${LIB_SOURCE_DIR}/warps/dsp/modulator.cc
        ${LIB_SOURCE_DIR}/warps/dsp/oscillator.cc
        ${LIB_SOURCE_DIR}/warps/dsp/vocoder.cc
        ${LIB_SOURCE_DIR}/warps/resources.cc
    )

    if (1)
        list(APPEND LIB_HEADERS
        # ${LIB_HEADER_DIR}/stmlib
        # ${LIB_HEADER_DIR}/stmlib/algorithms
        ${LIB_HEADER_DIR}/stmlib/algorithms/note_stack.h
        ${LIB_HEADER_DIR}/stmlib/algorithms/pattern_predictor.h
        ${LIB_HEADER_DIR}/stmlib/algorithms/tiny_map.h
        ${LIB_HEADER_DIR}/stmlib/algorithms/voice_allocator.h
        # ${LIB_HEADER_DIR}/stmlib/dsp
        ${LIB_HEADER_DIR}/stmlib/dsp/atan.h
        ${LIB_HEADER_DIR}/stmlib/dsp/cosine_oscillator.h
        ${LIB_HEADER_DIR}/stmlib/dsp/delay_line.h
        ${LIB_HEADER_DIR}/stmlib/dsp/dsp.h
        ${LIB_HEADER_DIR}/stmlib/dsp/filter.h
        ${LIB_HEADER_DIR}/stmlib/dsp/hysteresis_filter.h
        ${LIB_HEADER_DIR}/stmlib/dsp/hysteresis_quantizer.h
        ${LIB_HEADER_DIR}/stmlib/dsp/limiter.h
        ${LIB_HEADER_DIR}/stmlib/dsp/parameter_interpolator.h
        ${LIB_HEADER_DIR}/stmlib/dsp/polyblep.h
        ${LIB_HEADER_DIR}/stmlib/dsp/rsqrt.h
        ${LIB_HEADER_DIR}/stmlib/dsp/sample_rate_converter.h
        ${LIB_HEADER_DIR}/stmlib/dsp/units.h
        # ${LIB_HEADER_DIR}/stmlib/fft
        ${LIB_HEADER_DIR}/stmlib/fft/shy_fft.h
        # ${LIB_HEADER_DIR}/stmlib/LICENSE
        ${LIB_HEADER_DIR}/stmlib/stmlib.h
        # ${LIB_HEADER_DIR}/stmlib/utils
        ${LIB_HEADER_DIR}/stmlib/utils/buffer_allocator.h
        ${LIB_HEADER_DIR}/stmlib/utils/crc32.h
        ${LIB_HEADER_DIR}/stmlib/utils/dsp.h
        ${LIB_HEADER_DIR}/stmlib/utils/gate_flags.h
        ${LIB_HEADER_DIR}/stmlib/utils/murmurhash3.h
        ${LIB_HEADER_DIR}/stmlib/utils/random.h
        ${LIB_HEADER_DIR}/stmlib/utils/ring_buffer.h
        ${LIB_HEADER_DIR}/stmlib/utils/stream_buffer.h
        )
        list(APPEND LIB_SOURCES
        # ${LIB_SOURCE_DIR}/stmlib
        # ${LIB_SOURCE_DIR}/stmlib/dsp
        ${LIB_SOURCE_DIR}/stmlib/dsp/atan.cc
        ${LIB_SOURCE_DIR}/stmlib/dsp/units.cc
        # ${LIB_SOURCE_DIR}/stmlib/utils
        ${LIB_SOURCE_DIR}/stmlib/utils/random.cc
        )
    endif()

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC NOASM=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

    # set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/plugin.def")

    # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
    # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

endif()

# add_subdirectory(test)
