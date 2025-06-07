juce_add_plugin(sound0matic
    COMPANY_NAME "nonlinear_laboratories"
    BUNDLE_ID "com.nonlinearlaboratories.sound0matic"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE V01D
    PLUGIN_CODE S0M1
    FORMATS VST3
    PRODUCT_NAME "sound0matic"
)

#  This must come AFTER juce_add_plugin
juce_generate_juce_header(sound0matic)

target_link_libraries(sound0matic PRIVATE
    juce::juce_dsp
    juce::juce_audio_utils
    juce::juce_audio_processors 
)

target_sources(sound0matic PRIVATE
    PluginProcessor.cpp
    PluginEditor.cpp
    SampleLoader.cpp
    FFTProcessor.cpp
    Morphology.cpp
    STNModule.cpp
    PhaseVocoder.cpp
    SpectralFX.cpp
    PhaseFX.cpp
    PostFX.cpp
)

#  Optional but useful for VST3 host builds
target_compile_definitions(sound0matic PRIVATE JUCE_PLUGINHOST_VST3=1)

target_compile_options(sound0matic PRIVATE -Wno-multichar)

#  Leave this line at the bottom
install(TARGETS sound0matic DESTINATION .)
