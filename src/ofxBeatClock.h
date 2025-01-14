
//---------------------------------
//
//	OPTIONAL DEFINES

#define USE_ofxAbletonLink // -> Can be commented to not include the Ableton Link feature/add-on

//#define USE_AUDIO_BUFFER_TIMER_MODE // -> [WIP] Better alternative clock engine based on audio buffer 

//
//---------------------------------


//-

/*

	TODO:
		+	remove guiExtended
		+	remove native preview clock widget
		+ 	On-the-fly bang re-sync to bar beat start. (kind of manual syncer)
		+ 	Add fast filter to smooth / stabilize BPM number when using external midi clock mode.
		+ 	Add audio output selector to metronome sounds.
				maybe share audioBuffer with better timer mode
				on USE_AUDIO_BUFFER_TIMER_MODE. still disabled by default yet
	NOTE:
			more info about soundStream timer
			https://forum.openframeworks.cc/t/pass-this-pointer-from-parent-to-child-object-scheduler-oftimer-system/22088/6?u=moebiussurfing

*/

//-

/*

TODO:

- **BUG**: Repair problems when sometimes beat 1 tick it's displaced to beat 2...
- **BUG**: Some log errors must be repaired on ofxAbletonLink that seems drop down fps/performance...
- Add the correct workflow for LINK. Must add some mode toggle.
- On-the-fly re-sync to bar beat start.
- A better link between play button/params in all internal/external clock source modes, with one unique play button for all clock sources.
- Add alternative and better timer approach using the audio-buffer to avoid out-of-sync problems of current timers
(https://forum.openframeworks.cc/t/audio-programming-basics/34392/10).
Problems happen when minimizing or moving the app window.. Any help is welcome!
- Add kind of plugin to add audio input to get BeatTick on the fly from incomming audio signal. (Using ofxBTrack)

*/

//-

/// BUG: [1] ?
///sometimes metronome ticks goes on beat 2 instead 1.
///works better with 0 and 4 detectors, but why?
///SOLUTION:
///we must check better all the beat%limit bc should be the problem!!
///maybe we can add another beat_current varialbe, independent of the received beat from source clocks
///then to eliminate the all the limiters.
///must check each source clock type what's the starting beat: 0 or 1!!

/// BUG: [2] ?
/// kind of thread errors ?
///Exception thrown at 0x00007FF98CE1D759 in example_Link.exe: Microsoft C++ exception : std::system_error at memory location 0x0000000008B2F1F0.
///The thread 0x4338 has exited with code 0 (0x0).
///Exception thrown at 0x00007FF98CE1D759 in example_Link.exe: Microsoft C++ exception : std::system_error at memory location 0x0000000008B2F1F0.
///The thread 0x2a44 has exited with code 0 (0x0).
///Exception thrown at 0x00007FF98CE1D759 in example_Link.exe: Microsoft C++ exception : std::system_error at memory location 0x0000000008B2F1F0.
///The thread 0x2640 has exited with code 0 (0x0).
///[notice] ofApp: BeatTick !#2
///[notice] ofApp: BeatTick !#3

//----

#pragma once

#include "ofMain.h"


#include "ofxMidiClock.h" // used for external midi clock sync (1)
#include "ofxMidi.h"
#include "ofxMidiTimecode.h"
#include "ofxDawMetro.h" // used for internal (using threaded timer) clock (2)
#include "ofxSurfingHelpers.h"
#include "CircleBeat.h"
#include "BpmTapTempo.h"
#include "ofxInteractiveRect.h" // engine to move the gui. TODO: add resize by mouse too.

#define USE_OFX_SURFING_IM_GUI // -> can't be disabled for the momment. but is not difficult if you need it.
#ifdef USE_OFX_SURFING_IM_GUI
#include "ofxSurfingImGui.h"
#endif

//----

//* OPTIONAL : Ableton Link feature *

#ifdef USE_ofxAbletonLink
#include "ofxAbletonLink.h"//used for external Ableton Live Link engine (3)
#endif
///* Is the only external mode where OF app works as clock master. 
///(besides external midi sync slave)
///* It can control the Ableton bpm, play/stop etc from OF.
///What is Ableton Link?
///"This is the codebase for Ableton Link, a technology that synchronizes musical beat,
///tempo, and phase across multiple applications running on one or more devices.
///Applications on devices connected to a local network discover each other automatically
///and form a musical session in which each participant can perform independently: 
///anyone can start or stop while still staying in time.Anyone can change the tempo, 
///the others will follow.Anyone can join or leave without disrupting the session."
///https://github.com/Ableton/link -> original libs
///https://www.ableton.com/en/link/ -> videos and tutorials
///NOTES:(?)
///I don't understand yet what does when we make link.setBeat()...
///bc beat position of Ableton will not be updated changing this..
///So, it seems this is not the philosophy behind LINK:
///The idea of LINK seems to link the "gloabl" /phase/bar/beat to play live simultaneously
///many musicians or devices/apps, not to sync two long musics projects playing together.

//----

//* OPTIONAL : maybe better alternative internal clock *

///TODO:
//used as audioBuffer timer as an alternative for the internal clock (4)
///when it's enabled ofxDawMetro is not used and could be not loaded.
///WIP: alternative and better timer approach using the audio-buffer to avoid out-of-sync problems of current timers
///(https://forum.openframeworks.cc/t/audio-programming-basics/34392/10). 
///Problems happen when minimizing or moving the app window.. Any help is welcome!
///(code is at the bottom)
///un-comment to enable this NOT WORKING yet alternative mode
///THE PROBLEM: clock drift very often.. maybe bc wasapi sound api? ASIO seems a better choice.
///help on improve this is welcome!
///NOTE: if the audio output/driver is not opened properly, fps performance seems to fall...
///TODO: should make easier to select sound output

//----

///TODO:
///WIP
///smooth global bpm clock that is received from external fluctuating clocks
///could be done with only visual refreshing the midi clock slower,
///or using a real filter to the bpm variable.
///#define BPM_MIDI_CLOCK_REFRESH_RATE 1000
///refresh received MTC by clock. disabled/commented to "realtime" by every-frame-update

//----

//only to long song mode on external midi sync vs simpler 4 bars / 16 beats
#define ENABLE_PATTERN_LIMITING//comment to disable
#define PATTERN_STEP_BAR_LIMIT 4
#define PATTERN_STEP_BEAT_LIMIT 16//TODO: this are 16th ticks not beat!
#define BPM_INIT 120
#define BPM_INIT_MIN 40
#define BPM_INIT_MAX 400
#define USE_VISUAL_FEEDBACK_FADE//comment to try improve performance... Could add more optimizations maybe

//-

class ofxBeatClock : public ofBaseApp, public ofxMidiListener, public ofxDawMetro::MetroListener
{
public:
	ofxBeatClock();
	~ofxBeatClock();

	void setup();
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void exit();
	void windowResized(int w, int h);
	void keyPressed(ofKeyEventArgs &eventArgs);

	//-

#ifdef USE_OFX_SURFING_IM_GUI
	ofxSurfing_ImGui_Manager guiManager;
#endif

	//-

private:
	int window_W;
	int window_H;

	//-

private:
	void startup();
	void draw_PreviewWidget();

	CircleBeat circleBeat;
	BpmTapTempo bpmTapTempo;

	//-

private:
	ofParameter<bool> bKeys;

	ofxInteractiveRect rPreview = { "_BeatClock_Gui" };
	ofParameter<bool> MODE_Editor;
	ofParameter<bool> SHOW_Editor;
	std::string name_r1 = "_BeatClock";
	std::string name_r2 = "Gui_";
	const int padx = 10;
	const int pady = 30;
	//ofParameter<float> _rectRatio;
	//float _RectClick_w;
	//float _RectClick_Pad;

	//-

	// midi_in_clock

private:
	ofxMidiIn midiIn;
	ofxMidiClock midiIn_Clock;//< clock message parser

	ofxMidiMessage midiIn_Clock_Message;
	void newMidiMessage(ofxMidiMessage& eventArgs);

	double midiIn_Clock_Bpm;//< song tempo in bpm, computed from clock length
	bool bMidiInClockRunning;//< is the clock sync running?
	unsigned int MIDI_beats;//< song pos in beats
	int MIDI_quarters;//convert total # beats to # quarters
	int MIDI_bars;//compute # of bars
	double MIDI_seconds;//< song pos in seconds, computed from beats

	//TEST
	//int MIDI_ticks;//16th ticks are not implemented on the used ofxMidi example

	//-

	ofParameter<int> midiIn_BeatsInBar;//compute remainder as # TARGET_NOTES_params within the current bar
	void Changed_midiIn_BeatsInBar(int & beatsInBar);//only used in midiIn clock sync 
	int beatsInBar_PRE;//not required

	//-

	// external_midi_clock

	void setup_MidiIn_Clock();

	//-

	// layout

	//NOTE: all the layout system is a little messy yet. sometimes using glm, or x y points ...etc

private:
	bool DEBUG_moreInfo = false;//more debug

	ofParameter<glm::vec2> pos_Global;//main anchor to reference all the other above gui elements
	ofParameter<glm::vec2> pos_ClockInfo;
	ofParameter<glm::vec2> pos_BpmInfo;

	int pos_BeatBoxes_x, pos_BeatBoxes_y, pos_BeatBoxes_width;
	int pos_BeatBall_x, pos_BeatBall_y, pos_BeatBall_radius;

	//ofParameter<glm::vec2> pos_Gui;

	//TODO:
	//ofParameter<glm::vec2> position_BeatBoxes;
	//ofParameter<glm::vec2> position_BeatBall;
	//ofParameter<glm::vec2> position_ClockInfo;
	//ofParameter<glm::vec2> position_BpmInfo;

	//----

	//int gui_Panel_Width, gui_Panel_posX, gui_Panel_posY;

	//-

	// api setters

//public:
private:

	//void setPosition_GuiPanel(int x, int y, int w);//gui panel
	//ofPoint getPosition_GuiPanel();
	//glm::vec2 getPosition_GuiPanel();

	void setVisible_GuiPanel(bool b);
	void setVisible_GuiPreview(bool b);

	void setPosition_GuiGlobal(int x, int y);//main global position setter for gui panel and extra elements
	void setPosition_GuiPreviewWidget(int x, int y);//extra elements position setter with default layout of the other elements

	//--------------------------------------------------------------
	void setToggleVisible_GuiPreview()
	{
		bGui_PreviewClockNative = !bGui_PreviewClockNative;
	}

public:
	//--------------------------------------------------------------
	void toggleVisibleGui()
	{
		bGui = !bGui;
	}

private:
	void setPosition_BeatBoxes(int x, int y, int w);//position x, y and w = width of all 4 squares
	void setPosition_BeatBall(int x, int y, int w);//position x, y and w = width of ball
	void setPosition_ClockInfo(int _x, int _y);//all clock source info
	void setPosition_BpmInfo(int _x, int _y);//current bpm

	// beat boxes
	void draw_BeatBoxes(int x, int y, int w);

	// beat tick ball
	void draw_BeatBall(int x, int y, int radius);

	// beat tick ball
	void draw_ClockInfo(int x, int y);

	// beat tick ball
	void draw_BpmInfo(int x, int y);

	// big clock
	void draw_BigClockTime(int x, int y);

	void draw_ImGuiCircleBeatWidget();

	bool bb[4];
	ofColor cb[4];
	std::string strTimeBeatPos;

	//--

	// debug helpers

	// red anchor circle to debug mark
	bool DEBUG_Layout = false;
	//--------------------------------------------------------------
	void draw_Anchor(int x, int y)
	{
		if (DEBUG_Layout)
		{
			ofPushStyle();
			ofFill();
			ofSetColor(ofColor(0, 200));

			//circle
			ofDrawCircle(x, y, 3);

			//text
			int pad;
			if (y < 15) pad = 10;
			else pad = -10;
			ofDrawBitmapStringHighlight(ofToString(x) + "," + ofToString(y), x, y + pad);
			ofPopStyle();
		}
	}
	//--------------------------------------------------------------
	void setDebug_Clock(bool b)
	{
		DEBUG_moreInfo = b;
	}
	//--------------------------------------------------------------
	void toggleDebug_Clock()
	{
		DEBUG_moreInfo = !DEBUG_moreInfo;
	}
	//--------------------------------------------------------------
	void setDebug_Layout(bool b)
	{
		DEBUG_Layout = b;
	}
	//--------------------------------------------------------------
	void toggleDebug_Layout()
	{
		DEBUG_Layout = !DEBUG_Layout;
	}

	//--

private:
	// main text color
	ofColor colorText;

	//-

	// monitor_visual_feedback

private:
	//beat ball
	ofPoint circlePos;
	//float fadeOut_animTime, fadeOut_animCounter;
	//bool fadeOut_animRunning;
	float dt;
public:
	//--------------------------------------------------------------
	void setFrameRate(float _fps) {
		dt = 1.0f / _fps;
	}
private:
	//main receiver
	//trigs sound and gui drawing ball visual feedback
	void beatTick_MONITOR(int beat);//trigs ball drawing and sound ticker

	int lastBeatFlash = -1;
	//void draw_BeatBalFlash(int _onBeat){}

public:
	ofParameter<bool> BeatTick;//get bang beat!!
	//also this trigs to draw a flashing circle for a frame only
	//this variable is used to subscribe external (in ofApp) listeners to get the beat bangs!

	//-

////TODO:
////WIP
//private:
//smooth clock for midi input clock sync
////  REFRESH_FEQUENCY
////used only when BPM_MIDI_CLOCK_REFRESH_RATE is defined
//unsigned long BPM_LAST_Tick_Time_LAST;//test
//unsigned long BPM_LAST_Tick_Time_ELLAPSED;//test
//unsigned long BPM_LAST_Tick_Time_ELLAPSED_PRE;//test
//long ELLAPSED_diff;//test
//unsigned long bpm_CheckUpdated_lastTime;

	//-

public:

	void setup_ImGui();
	void refresh_Gui();
	void refresh_GuiWidgets();

#ifdef USE_OFX_SURFING_IM_GUI
	void draw_ImGuiWidgets();
	void draw_ImGuiControl();
	void draw_ImGuiClockMonitor();
	void draw_ImGuiBpmClock();
#endif

	//-

private:
	ofParameterGroup params_INTERNAL;
	ofParameterGroup params_EXTERNAL_MIDI;
	ofParameterGroup params_BPM_Clock;

	ofJson confg_Button_C, confg_Button_L, confg_ButtonSmall, confg_Sliders;//json theme

	//-

	// params
public:
	ofParameter<bool> PLAYING_Global_State;//for all different source clock modes
	ofParameter<bool> bGui;

private:
	ofParameterGroup params_CONTROL;

	ofParameter<bool> PLAYING_Internal_State;//player state only for internal clock
	ofParameter<bool> PLAYING_External_State;//player state only for external clock

	ofParameter<bool> ENABLE_CLOCKS;//enable clock (affects all clock types)
	ofParameter<bool> MODE_INTERNAL_CLOCK;//enable internal clock
	ofParameter<bool> MODE_EXTERNAL_MIDI_CLOCK;//enable midi clock sync
	ofParameter<int> midiIn_Port_SELECT;
	int midiIn_numPorts = 0;

	//----

public:
	// this is the main and final target bpm, is the destination of all other clocks (internal, external midi sync or ableton link)
	ofParameter<float> BPM_Global;//global tempo bpm.
	ofParameter<int> BPM_Global_TimeBar;//ms time of 1 bar = 4 beats

	//----

private:
	ofParameter<bool> BPM_Tap_Tempo_TRIG;//trig the measurements of tap tempo

	// helpers to modify current bpm
	ofParameter<bool> RESET_BPM_Global;
	ofParameter<bool> BPM_half_TRIG;//divide bpm by 2
	ofParameter<bool> BPM_double_TRIG;//multiply bpm by 2

	//-

	//API

public:
	float getBPM();//returns BPM_Global
	int getTimeBar();//returns duration of global bar in ms

	int getBeat() {
		if (BeatTick) {
			//ofLogNotice(__FUNCTION__) << "BeatTick ! #" << Beat_current;
			return Beat_current;
		}
		else return -1;
	}

	//--

	// this methods could be useful only to visualfeedback on integrating to other bigger guis on ofApp..
	//--------------------------------------------------------------
	bool getInternalClockModeState()
	{
		return MODE_INTERNAL_CLOCK;
	}
	//--------------------------------------------------------------
	bool getExternalClockModeState()
	{
		return MODE_EXTERNAL_MIDI_CLOCK;
	}
#ifdef USE_ofxAbletonLink
	//--------------------------------------------------------------
	bool getLinkClockModeState()
	{
		return MODE_ABLETON_LINK_SYNC;
	}
#endif

	//--

private:
	// main callback handler
	void Changed_Params(ofAbstractParameter& e);

	//-

	// internal clock

	// based on threaded timer using ofxDawMetro
	// internal_clock

	ofxDawMetro clockInternal;

	// callbacks defined inside the addon class. can't be renamed here
	// overide ofxDawMetro::MetroListener's method if necessary
	void onBarEvent(int & bar) override;
	void onBeatEvent(int & beat) override;
	void onSixteenthEvent(int & sixteenth) override;

	ofParameter<float> BPM_ClockInternal;

	//NOTE: 
	//for the momment this bpm variables is being used as
	//main tempo (sometimes) for other clock sources too.
	//TODO:
	//Maybe we should improve this using global bpm variable (BPM_Global) as main.

	ofParameterGroup params_ClockInternal;
	ofParameter<bool> clockInternal_Active;
	void Changed_ClockInternal_Bpm(float & value);
	void Changed_ClockInternal_Active(bool & value);

	//-

	////TODO:
	//void reSync();
	//ofParameter<bool> bSync_Trig;

	//-

public:
	void setBpm_ClockInternal(float bpm); // to set bpm from outside

	//-

	// settings
private:
	std::string path_Global;

public:
	//--------------------------------------------------------------
	void setPathglobal(std::string _path) {
		path_Global = _path;

		ofxSurfingHelpers::CheckFolder(path_Global);
	}

private:
	void saveSettings(std::string path);
	void loadSettings(std::string path);

	std::string file_BeatClock = "BeatClock_Settings.xml";
	std::string file_Midi = "Midi_Settings.xml";
	std::string file_App = "App_Settings.xml";

	ofParameterGroup params_App;

	//-

	// fonts
	std::string strBpmInfo;
	std::string strTapTempo;
	std::string strExtMidiClock;
	std::string strClock;
	std::string strLink;
	std::string strMessageInfo;
	std::string strDebugInfo;
	std::string strMessageInfoFull;
	ofTrueTypeFont fontSmall;
	ofTrueTypeFont fontMedium;
	ofTrueTypeFont fontBig;

	//-

	// beat ball
	ofPoint metronome_ball_pos;
	int metronome_ball_radius;

	//-

	ofParameter<bool> bGui_Transport;
	ofParameter<bool> bGui_PreviewClockNative; // beat boxes, text info and beat ball (all except gui panels)
	ofParameter<bool> bGui_BpmClock; // some helpers other secondary settings/controls 
	glm::vec2 shapePreview;
	void draw_PreviewWidgetItems();

	//-

	//sound metronome
	ofParameter<bool> ENABLE_sound; // enable sound ticks
	ofParameter<float> volumeSound; // sound ticks volume

	//-

	// current_bpm_clock_values
public:
	void reset_ClockValues(); // set gui display text clock to 0:0:0

	//TODO: could be nice to add some listener system..
	ofParameter<int> Bar_current;
	ofParameter<int> Beat_current;
	ofParameter<int> Tick_16th_current; // used only with audioBuffer timer mode

	//TODO: 
	//link
#ifdef USE_ofxAbletonLink
	int Beat_current_PRE; // used to detect changes only on link mode
	float Beat_float_current; // link beat received  with decimals (float) and starting from 0 not 1 
	std::string Beat_float_string;
#endif

private:

	// strings for monitor drawing
	//1:1:1
	std::string Bar_string;
	std::string Beat_string;
	std::string Tick_16th_string;

	std::string clockActive_Type; // internal/external/link clock types name
	std::string clockActive_Info; // midi in port, and extra info for any clock source

	//----

	// API

	//----

public:

	// main transport control for master mode (not in external midi sync that OF app is slave)
	void start(); // only used on internal or link clock source mode
	void stop(); // only used on internal or link clock source mode
	void setTogglePlay(); // only used on internal or link clock source mode

	//bool isPlaying()
	//{
	//	return bIsPlaying;
	//}

	//----

private:

	//bool bIsPlaying; // used only for internal clock mode.. should be usefull for all clock types

	//----

	// tap_engine

public:
	void tap_Trig();
	void tap_Update();

private:
	float tap_BPM;
	//int tap_Count, tap_LastTime, tap_AvgBarMillis;
	//vector<int> tap_Intervals;
	//bool bTap_Running;
	bool SOUND_wasDisabled = false; // sound disbler to better user workflow

	//----

	// change_midi_in_port

	void setup_MidiIn_Port(int p);
	int midiIn_Clock_Port_OPENED;
	int midiIn_Port_PRE = -1;
	ofParameter<std::string> midiIn_PortName{ "","" };

	//----

	// step limiting

	// we don't need to use long song patterns
	// and we will limit bars to 4 like a simple step sequencer.
	bool ENABLE_pattern_limits;
	int pattern_BEAT_limit;
	int pattern_BAR_limit;

	//----

	//TODO:
	// audioBuffer alternative timer mode to get a a more accurate clock!
	// based on:
	// https://forum.openframeworks.cc/t/audio-programming-basics/34392/10?u=moebiussurfing
	// by davidspry:
	// "the way I�m generating the clock is naive and simple.I�m simply counting the number of samples written to the buffer and sending a notification each time the number of samples is equal to one subdivided �beat�, as in BPM.
	// Presumably there�s some inconsistency because the rate of writing samples to the buffer is faster than the sample rate, but it seems fairly steady with a small buffer size."

	// NOTE: we will want maybe to use the same soundStream used for the sound tick also for the audiBuffer timer..
	// maybe will be a better solution to put this timer into ofxDawMetro class!!

#ifdef USE_AUDIO_BUFFER_TIMER_MODE
private:
	void setupAudioBuffer(int _device);
	void closeAudioBuffer();
	ofParameter<bool> MODE_AudioBufferTimer;
	ofSoundStream soundStream;
	int deviceOut;
	int samples = 0;
	int ticksPerBeat = 4;
	//default is 4. 
	//is this a kind of resolution if we set bigger like 16?
	int samplesPerTick;
	int sampleRate;
	int bufferSize;
	int DEBUG_ticks = 0;
	bool DEBUG_bufferAudio = false;
public:
	void audioOut(ofSoundBuffer &buffer);
#endif

	//----

#ifdef USE_ofxAbletonLink
private:

	ofxAbletonLink link;

	ofParameter<bool> MODE_ABLETON_LINK_SYNC;

	ofParameterGroup params_LINK;

	////TODO. test knobs
	////ofParameter<float> valueKnob{ "value", 0.f, -10.f, 10.0f };
	//ofParameter<float> valueKnob{ "value", 0.5f, 0.f, 1.0f };

	ofParameter<bool> LINK_Enable;//enable link
	ofParameter<float> BPM_Link;//link bpm
	ofParameter<bool> PLAYING_Link_State;//control and get Ableton Live playing too, mirrored like Link does
	ofParameter<float> LINK_Phase;//phase on the bar. cycled from 0.0f to 4.0f
	ofParameter<bool> LINK_ResyncBeat;//set beat 0
	ofParameter<bool> LINK_ResetBeats;//reset "unlimited-growing" beat counter
	ofParameter<std::string> LINK_Beat_string;//monitor beat counter
	//amount of beats are not limited nor in sync / mirrored with Ableton Live.
	ofParameter<std::string> LINK_Peers_string;//number of synced devices/apps on your network
	//ofParameter<int> LINK_Beat_Selector;//TODO: TEST
	//int LINK_Beat_Selector_PRE = -1;

	//--------------------------------------------------------------
	void LINK_setup()
	{
		link.setup();

		ofAddListener(params_LINK.parameterChangedE(), this, &ofxBeatClock::Changed_LINK_Params);
	}

	//--------------------------------------------------------------
	void LINK_update()
	{
		if (MODE_ABLETON_LINK_SYNC)//not required but prophylactic
		{
			//display text
			clockActive_Type = "ABLETON LINK";

			clockActive_Info = "BEAT   " + ofToString(link.getBeat(), 1);
			clockActive_Info += "\n";
			clockActive_Info += "PHASE  " + ofToString(link.getPhase(), 1);
			clockActive_Info += "\n";
			clockActive_Info += "PEERS  " + ofToString(link.getNumPeers());

			//-

			//assign link states to our variables

			if (LINK_Enable && (link.getNumPeers() != 0))
			{
				LINK_Phase = link.getPhase();//link bar phase

				Beat_float_current = (float)link.getBeat();
				Beat_float_string = ofToString(Beat_float_current, 2);

				LINK_Beat_string = ofToString(link.getBeat(), 0);//link beat with decimal
				//amount of beats are not limited nor in sync / mirrored with Ableton Live.
			}

			//---

			//update mean clock counters and update gui
			if (LINK_Enable && PLAYING_Link_State && (link.getNumPeers() != 0))
			{
				int _beats = (int)Beat_float_current;//starts in beat 0 not 1

				//-

				//beat
				if (ENABLE_pattern_limits)
				{
					Beat_current = 1 + (_beats % 4);//limited to 4 beats. starts in 1
				}
				else
				{
					Beat_current = 1 + (_beats);
				}
				Beat_string = ofToString(Beat_current);

				//-

				//bar
				int _bars = _beats / 4;
				if (ENABLE_pattern_limits)
				{
					Bar_current = 1 + _bars % pattern_BAR_limit;
				}
				else
				{
					Bar_current = 1 + _bars;
				}
				Bar_string = ofToString(Bar_current);

				//---

				if (Beat_current != Beat_current_PRE)
				{
					ofLogVerbose(__FUNCTION__) << "LINK beat changed:" << Beat_current;
					Beat_current_PRE = Beat_current;

					//-

					beatTick_MONITOR(Beat_current);

					//-
				}
			}
		}

		//-

		// blink gui label if link is not connected! (no peers)
		// to alert user to re click LINK buttons(in OF app and Ableton too)
		if (link.getNumPeers() == 0)
		{
			if ((ofGetFrameNum() % 60) < 30)
			{
				LINK_Peers_string = "0";
			}
			else
			{
				LINK_Peers_string = " ";
			}
		}
	}

	//--------------------------------------------------------------
	void LINK_draw()
	{
		ofPushStyle();

		// text
		int xpos = rPreview.getX() + 20;
		int ypos = rPreview.getBottomLeft().y + 30;
		//int xpos = 20;
		//int ypos = 20;

		// line
		float x = ofGetWidth() * link.getPhase() / link.getQuantum();

		// red vertical line
		ofSetColor(255, 0, 0);
		ofDrawLine(x, 0, x, ofGetHeight());

		std::stringstream ss("");
		ss
			<< "Bpm  : " << ofToString(link.getBPM(), 2) << std::endl
			<< "Beat : " << ofToString(link.getBeat(), 2) << std::endl
			<< "Phase: " << ofToString(link.getPhase(), 2) << std::endl
			<< "Peers: " << link.getNumPeers() << std::endl
			<< "Play?: " << (link.isPlaying() ? "play" : "stop");

		ofSetColor(255);
		if (fontMedium.isLoaded())
		{
			fontMedium.drawString(ss.str(), xpos, ypos);
		}
		else
		{
			ofDrawBitmapString(ss.str(), xpos, ypos);
		}

		ofPopStyle();
	}

	//--------------------------------------------------------------
	void LINK_exit()
	{
		ofLogNotice(__FUNCTION__) << "Remove LINK listeners";
		ofRemoveListener(link.bpmChanged, this, &ofxBeatClock::LINK_bpmChanged);
		ofRemoveListener(link.numPeersChanged, this, &ofxBeatClock::LINK_numPeersChanged);
		ofRemoveListener(link.playStateChanged, this, &ofxBeatClock::LINK_playStateChanged);

		ofRemoveListener(params_LINK.parameterChangedE(), this, &ofxBeatClock::Changed_LINK_Params);
	}

	//--

	// callbacks

	//--------------------------------------------------------------
	void Changed_LINK_Params(ofAbstractParameter &e)
	{
		std::string name = e.getName();

		if (name != "PEERS") // exclude log
		{
			ofLogVerbose(__FUNCTION__) << name << " : " << e;
		}

		//-

		if (name == "PLAY")
		{
			ofLogNotice(__FUNCTION__) << "LINK PLAY: " << PLAYING_Link_State;

			if (LINK_Enable && (link.getNumPeers() != 0))
			{
				//TODO:
				// BUG:
				// play engine do not works fine

				//TEST:
				if (link.isPlaying() != PLAYING_Link_State) // don't need update if it's already "mirrored"
				{
					link.setIsPlaying(PLAYING_Link_State);
				}

				////TEST:
				//if (PLAYING_Link_State)
				//{
				//	link.play();
				//}
				//else
				//{
				//	link.stop();
				//}

				// workflow
				// set gui display text clock to 0:0:0
				if (!PLAYING_Link_State)
				{
					reset_ClockValues();
				}
			}
			// workflow
			else if (PLAYING_Link_State)
			{
				PLAYING_Link_State = false; // if not enable block to play disabled
			}
		}

		else if (name == "LINK")
		{
			ofLogNotice(__FUNCTION__) << "LINK: " << LINK_Enable;

			//TEST:
			//if (LINK_Enable)
			//{
			//	link.enablePlayStateSync();
			//}
			//else
			//{
			//	link.disablePlayStateSync();
			//}

			//TEST:
			if (LINK_Enable)
			{
				link.enableLink();
			}
			else
			{
				link.disableLink();

				// workflow
				if (PLAYING_Link_State)
				{
					PLAYING_Link_State = false; // if not enable block to play disabled
				}
			}
		}

		else if (name == "BPM" && LINK_Enable)
		{
			ofLogNotice(__FUNCTION__) << "LINK BPM";

			if (link.getBPM() != BPM_Link)
			{
				link.setBPM(BPM_Link);
			}

			if (MODE_ABLETON_LINK_SYNC)
			{
				//TODO: 
				// it's required if ofxDawMetro is not being used?
				BPM_ClockInternal = BPM_Link;

				// will be autoupdate on clockInternal callback
				//BPM_Global = BPM_Link;
			}
		}

		else if (name == "RESYNC" && LINK_ResyncBeat && LINK_Enable)
		{
			ofLogNotice(__FUNCTION__) << "LINK RESTART";
			LINK_ResyncBeat = false;

			link.setBeat(0.0);

			if (MODE_ABLETON_LINK_SYNC)
			{
				Tick_16th_current = 0;
				Tick_16th_string = ofToString(Tick_16th_current);

				Beat_current = 0;
				Beat_string = ofToString(Beat_current);

				Bar_current = 0;
				Bar_string = ofToString(Bar_current);
			}
		}

		else if (name == "FORCE RESET" && LINK_ResetBeats && LINK_Enable)
		{
			ofLogNotice(__FUNCTION__) << "LINK RESET";
			LINK_ResetBeats = false;

			link.setBeatForce(0.0);
		}

		//TODO:
		// I don't understand yet what setBeat does...
		// bc beat position of Ableton will not be updated changing this..
		// So, it seems this is not the philosophy behind LINK:
		// The idea of LINK seems to link the bar/beat to play live simultaneously
		// many musicians or devices/apps

		//else if (name == "GO BEAT" && LINK_Enable)
		//{
		//	if (LINK_Beat_Selector != LINK_Beat_Selector_PRE)//changed
		//	{
		//		ofLogNotice(__FUNCTION__) << "LINK GO BEAT: " << LINK_Beat_Selector;
		//		LINK_Beat_Selector_PRE = LINK_Beat_Selector;
		//
		//		link.setBeat(LINK_Beat_Selector);
		//
		//		if (MODE_ABLETON_LINK_SYNC)
		//		{
		//			//Tick_16th_current = 0;
		//			//Tick_16th_string = ofToString(Tick_16th_current);
		//
		//			Beat_current = 0;
		//			Beat_string = ofToString(Beat_current);
		//		}
		//	}
		//}
	}

	// receive from master device (i.e Ableton Live)
	//--------------------------------------------------------------
	void LINK_bpmChanged(double &bpm)
	{
		ofLogNotice(__FUNCTION__) << bpm;

		BPM_Link = bpm;

		// BPM_Global will be update on the BPM_Link callback
		// BPM_ClockInternal will be updated too
	}

	//--------------------------------------------------------------
	void LINK_numPeersChanged(std::size_t &peers)
	{
		ofLogNotice(__FUNCTION__) << peers;
		LINK_Peers_string = ofToString(peers);

		if (peers == 0)
		{
			LINK_Phase = 0.f;
		}
	}

	//--------------------------------------------------------------
	void LINK_playStateChanged(bool &state)
	{
		ofLogNotice(__FUNCTION__) << (state ? "play" : "stop");

		// don't need update if it's already "mirrored"
		if (state != PLAYING_Link_State && MODE_ABLETON_LINK_SYNC && LINK_Enable)
		{
			PLAYING_Link_State = state;
		}
	}
#endif
};

