// Audio plugin header file with voice killing for synthesizers and support for processing silence for audio effect units.
// Use the entry point name main() for 100% compabillity
#ifdef __x86_64
	typedef int64_t plug;
	typedef int32_t plug_v;
#else
	typedef int32_t plug;
	typedef int16_t plug_v;
#endif

enum { LEFT, RIGHT }; // 0 for left audio channel, 1 for right audio channel.

typedef struct plug_header plug_header;
struct plug_header{ // The ABI for audio effect plugins
	int32_t magic_number_1;		// 1450406992
	plug    (*plug_instruction_decoder)(plug_header* effect, int32_t opcode, int32_t index, plug value, void* ptr, float opt);
	plug    dont_use_0; 		// not used, set to zero.
	void    (*plug_parameter_set)(plug_header* effect, int32_t index, float parameter);
	float   (*plug_parameter_get)(plug_header* effect, int32_t index);
	int32_t number_of_programs;	// number of programs in plug
	int32_t number_of_variables;	// all programs are assumed to have number_of_variables as the total number of parameters/var
	int32_t number_of_inputs;	// number of audio inputs in plug
	int32_t number_of_outputs;	// number of audio outputs in plug
	int32_t flags;			// see plug_flags
	plug    for_daw[2];		// plug shall not tamper with this after init.  
	int32_t plug_delay;		// plug latency in samples
	int32_t dont_use_1;		// Set to zero.
	int32_t dont_use_2;		// Set to zero.
	int32_t dont_use_3;		// Set to zero.
	void*   object_ptr;		// Pointer to this object/struct.
	void*   for_plug;		// Pointer for plug usage.
	int32_t dont_use_4;		// Set to zero.
	int32_t plug_version; 		// Version of this plug (will force updates about plug in host).
	void (*plug_float_audio) (plug_header* effect,  float** inputs,  float** outputs, int32_t samples); // Used by host to make plug process 32bit audio buffers (float).
	void (*plug_double_audio)(plug_header* effect, double** inputs, double** outputs, int32_t samples); // Used by host to make plug process 64bit audio buffers(double).
	int32_t magic_number_2;		// 'RST '
	// Support version data
	int32_t supported_version_lowest;  // Lowest supported RST ABI version. Set to 1000
	int32_t supported_version_highest; // Highest supported ABI version.    Set to 1000
	// SS "Silence Support"
	int32_t (*plug_float_audio_ss) (plug_header* effect,  float** inputs,  float** outputs, int32_t samples, int32_t silence); // First channel is bit 0, second is 1 annd so on for incomming value in silence and returned value. Flags is set for silent channels.
	int32_t (*plug_double_audio_ss)(plug_header* effect, double** inputs, double** outputs, int32_t samples, int32_t silence); // First channel is bit 0, second is 1 annd so on for incomming value in silence and returned value. Flags is set for silent channels.
	char reserved_block[100];	// reserved, set to zero
};
typedef plug (*daw_callback) (plug_header* effect, int32_t opcode, int32_t index, plug value, void* ptr, float opt); 

enum plug_flags{
	HAS_EDITOR	  = 1,		// Plug has an editor window.
	HAS_FLOAT_AUDIO	  = 16,		// Plug has function for writing audio out over the audio in buffer in 32bit floating point (float).
	HAS_DOUBLE_AUDIO  = 4096,	// Plug has function for writing audio out over the audio in buffer in 64bit floating point (double).
	HAS_SAVE_STATE	  = 32,		// internal state in plug can be copied to host, for saving and loading audio projects in host.
	HAS_CHANNEL_STRIP = 256, 	// Plug is a synthesizer. And 2 first channels will not be used for effect-send, and will have a channel strip for the 2 first channels.
	HAS_ONLY_SOUND_OUT_ON_SOUND_IN = 512, // To save processing power in host if possible.
	HAS_SILENCE_SUPPORT = 8192,
};
enum opcodes{
	PLUG_EDITOR_OPEN=14,		// Attention: Is called when the DAW want the plug to open a window.
	PLUG_EDITOR_CLOSE=15,		// Attention: After you answered this call, then the window is closed by DAW.
	PLUG_EDITOR_SIZE=13,		// Width and hight of the editor window.
	PLUG_EDITOR_REDRAW=19,		// Host tells plug to redraw graphics.
	PLUG_OPEN=0,			// Host warns it will start to use this plug instance before it does.
	PLUG_CLOSE=1,			// Host warns that it will end the usage of this plug instance.
	PLUG_GET_VAR_TEXT=7,		// Host asks for a text to display for the value of a parameter.
	PLUG_GET_VAR_NAME=8,		// Host asks for the name of a parameter in plug.
	PLUG_SET_SAMPLERATE=10,		// The host tells the plug what sample rate that is currently used.
	PLUG_GET_STATE=23,		// Host asks for an adress and the length of a data chunk that the host shall save for the plug.
	PLUG_SET_STATE=24,		// Host asks for an adress and length of the data segment that the host shall fill to restore an old state.
	PLUG_VALUE_AUTOMATION=26,	// For the host to check if it can automate a certain parameter.
	PLUG_GET_AUTHOR_TEXT=47,	// For the host to ask for the name of the author of the plug.
	PLUG_GET_PRODUCT_TEXT=48,	// For the host to ask for the name of the plug.	
	PLUG_SUPPORTS=51,		// For the host to ask if the plug supports a sertain functionallity, the name of the functionallity is supplied in a text string by the host.
	PLUG_GET_ABI_MAGIC_NUMBER=58,	// Asks the plug what version of the ABI that is was designed for, allways return 2400.
	PLUG_GET_PATCH_TEXT_BY_INDEX=29,// Gets the preset name by index.
	PLUG_GET_PATCH_TEXT=5,		// Alternative to plug_get_indexed_program_text.
	PLUG_SET_PATCH_NUMBER=2,	// Change plug preset program/patch.
	PLUG_GET_PATCH_NUMBER=3,	// Ask plug for program/patch number.
	PLUG_GET_PLUG_TYPE=35,		// Ask if it's a effect unit or a synthesizer.
	PLUG_SET_MIDI=25		// Plug recives MIDI events from host.
};
enum opcodesdaw{
	DAW_AUTOMATE=0,		// send knob value to host 
	DAW_AUTOMATE_START=43,	// tell host index of grabbed knob/var
	DAW_AUTOMATE_END=44,	// tell host about released knob/var
	DAW_GET_TIME=7,		// get time info from host
	DAW_SET_MIDI=8,		// MIDI data for host to recive from plug.
};
struct winrect{ 	// RECT structure same as in (windef.h)
	int16_t top;	// Set to zero - Specifies the y-coordinate of the upper-left corner of the rectangle.
	int16_t left;	// Set to zero - Specifies the x-coordinate of the upper-left corner of the rectangle.
	int16_t bottom;	// Set to window width - Specifies the x-coordinate of the lower-right corner of the rectangle.
	int16_t right;	// Set to window higth - Specifies the y-coordinate of the lower-right corner of the rectangle.
};
typedef struct midi_event{ // It's a special type of event
	int32_t event_type;		// Set to 1 if MIDI event, 6 is it's a sysex message that is not supported by this struct. (set to 7)
	int32_t event_size;		// Size of this event in bytes and that is 32.
	int32_t sample_offset;		// offset in number of samples in the audio buffer.
	int32_t realtime_flag;		// Set to 1 if it's not playbacked from a sequencer track. Handle the event immediately if set. if 0 check sample_offset for timing.
	int32_t note_length;		// Note length in samples if availible. Set to 0 if not used.
	int32_t dont_use_0;		// Is set to zero. 
	char MIDI_byte[4];		// 1 to 3 MIDI bytes depending on MIDI message. MIDIByte[3] is not used or is used for MIDI 2.0 if supported.
	char cents_detune;		// -64 to +63 cents 'microtuning'
	char off_velocity;		// Note Off Velocity, usefull when paired with note_length.
	char dont_use_1;		// Is set to zero. <- possible useage for note_id for a note kill bit.
	char dont_use_2;		// Is set to zero. <- possible useage for note_id
} midi_event;
typedef struct midi_events{
	int32_t number_of_events;	// number of plugMidiEvent's in MidiEvent[2]
	plug_v note_id;			// Is set to zero, if not used or supported.
	plug_v note_id_end;		// disipate and kill voice.
	plugMidiEvent* MIDI_event[2];	// hold one or two events depending on number_of_events
} midi_events;
typedef struct daw_transport{ // BPM is same as number as quarter notes per minute. PPQ is positions per quarter note. Values is relative to the start of the song.
	double  play_sample;			// The position in samples last played/playing.
	double  play_samplerate;		// The current sample rate.
	double  system_clock_ns;		// System clock in nano seconds.
	double  play_qp;			// The 'quarter note position' playing/last played.
	double  play_bpm;			// The current BPM (beats per minute) Is a beat is a quarter note in lengt.
	double  play_qp_start;			// The 'quarter note position' start position (into the song) last played.
	double  loop_qp_start;			// The 'quarter note position' left loop point.
	double  loop_qp_end;			// The 'quarter note position' right loop point.
	int32_t time_signature_upper_numeral;	// Like 16 in 16 / 4 tempo.
	int32_t time_signature_lower_numeral;	// Like 4 in 16/4 tempo.
	int32_t smpte_frame_offset;		// One frame to the next has a total value of 80. Thus the maximum value is 79 as 0 is exactly on frame uppdate.
	int32_t smpte_framerate;		// the number of frames per second.
	int32_t MIDI_clock_sample_offset;	// The closest MIDI clock that will/has occured. Signed +/- value.
	int32_t flags;				// On/off values, Use enum transport_flags to translate. eg. if(daw_transport->flags & DAW_PLAYING){ will tell you if the DAW is playing or not }.
}daw_transport;
enum transport_flags{
	DAW_CHANGED   		= 1,	// If if transport has changed since last read
	DAW_PLAYING   		= 2,	// Set if DAW is playing
	DAW_LOOPING   		= 4,	// Set if DAW has loop activated
	DAW_RECORDING 		= 8,	// Set if DAW is recording

	DAW_AUTOMATION_WRITE	= 64,	// DAW is writing automation to plug
	DAW_AUTOMATION_READ	= 128,	// DAW is reading autiomation from plug

	// play_sample value is always valid and has no bit flag.
	VALID_SYSTEM_CLOCK_ns		= 256,	
	VALID_PLAY_PPQ			= 512,	
	VALID_PLAY_BPM			= 1024,	
	VALID_PPQ_PLAY_START		= 2048,	
	VALID_PPQ_LOOP			= 4096,	
	VALID_TIME_SIGNATURE		= 8192,	
	VALID_SMPTE			= 16384,
	VALID_MIDI_CLOCK_SAMPLE_OFFSET	= 32768,
};
