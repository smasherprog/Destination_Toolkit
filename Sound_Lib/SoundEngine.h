#ifndef Sound_Engine_H
#define Sound_Engine_H
#pragma warning(push)
#pragma warning( disable : 4005 )
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>

#include "D3DX10Math.h"
#pragma warning(pop)
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../Utilities/MyMath.h"
#include "../Utilities/Utilities.h"

#include <iostream>
#if defined(_MSC_VER)
	#if defined(_DEBUG) | defined (DEBUG)
		#include <dxerr.h>
		#pragma comment(lib, "dxerr") // needed for dxtrace
	#endif
#endif

#if defined(_DEBUG) | defined (DEBUG)

#ifndef HR
#define HR(x){																\
	HRESULT hr = (x);														\
	if(FAILED(hr)){															\
	std::cout<<DXGetErrorStringA(hr)<<std::endl<<DXGetErrorDescriptionA(hr)<<std::endl;   \
	assert(0);															\
	}																		\
				}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

#if defined(_MSC_VER)
	#pragma comment(lib, "x3daudio")
#endif
#ifndef assert2
	#ifdef  NDEBUG	
	#define assert2(_Expression, _Msg) ((void)0)
	#else 
	#define assert2(_Expression, _Msg) (void)( (!!(_Expression)) || (_wassert(_Msg, _CRT_WIDE(__FILE__), __LINE__), 0) )
	#endif
#endif
#define INPUTCHANNELS 1  // number of source channels
#define OUTPUTCHANNELS 8 // maximum number of destination channels supported in this sample
#define NUM_PRESETS 30

namespace Sound_Engine {
	class SoundEmitter;
	class Sound;
	namespace Internal {
		void Init();
		void DeInit();
		void Update(float dt);
		void SetReverb( uint32_t nReverb );
		void SetListenerPositionAndOrientation(vec3& pos, vec3 look, vec3 up);

		class Sub_Sound{
		public:
			Sub_Sound();
			~Sub_Sound();
			XAUDIO2_BUFFER_WMA wmaBuffer;
			WAVEFORMATEXTENSIBLE wfx;
			XAUDIO2_BUFFER buffer;
			std::string File_Name;

		};
		//2d sound stuff
		extern IXAudio2* pXAudio2;
		extern IXAudio2MasteringVoice* pMasteringVoice;
		extern IXAudio2SubmixVoice* pSubmixVoice;
		extern IUnknown* pReverbEffect;

		//3d stuff
		extern X3DAUDIO_HANDLE x3DInstance;
		extern int nFrameToApply3DAudio;

		extern DWORD dwChannelMask;
		extern UINT32 nChannels;

		extern X3DAUDIO_DSP_SETTINGS dspSettings;
		extern X3DAUDIO_LISTENER listener;
		extern std::vector<std::shared_ptr<SoundEmitter>> Emitters;
		extern std::map<std::string, std::weak_ptr<Sub_Sound>> LoadedSounds;
		extern std::map<Sound*, std::weak_ptr<Sound>> Sources;
		const std::string Asset_Dir = "..\\Assets\\";
		extern D3DXVECTOR3 CurrentListenerPos;
		extern FLOAT32 matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
	};

	class Sound{
	public:
		Sound();
		~Sound();
		void Pause();
		void Stop();
		void Play(bool loopforever = true);
		void Resume();

		bool xWMA(){ return InternalSound->wmaBuffer.pDecodedPacketCumulativeBytes != 0; }

		IXAudio2SourceVoice* SourceVoice;
		std::shared_ptr<Internal::Sub_Sound> InternalSound;

	};

	class SoundEmitter {
	public:
		SoundEmitter(): Active(false){}
		X3DAUDIO_EMITTER emitter;
		X3DAUDIO_CONE emitterCone;
		D3DXVECTOR3 Pos;
		FLOAT32 emitterAzimuths[INPUTCHANNELS];
		bool Active;
		std::shared_ptr<Sound> sound;
	};

	void Init();
	void DeInit();
	void Update(float dt);
	void SetReverb( uint32_t nReverb );
	void SetListenerPositionAndOrientation(vec3& pos, vec3 look= vec3(0, 0, 1), vec3 up= vec3(0, 1, 0));

	// There is a difference between pause and stop. Pause will not clear the buffers. So, if you pause a song, then in another part of the program, start playing
	// the same song, it will start playing at the beginning, and where ever the last sound was paused, at the same time. Stop will compleltly reset the sound so next time you 
	// start the sound, it will be as brand new. Stop might cause a crackle sound, So its best to issue, a pause, followed by a stop on the next loop.
	void PauseAll();
	void ResumeAll();
	void StopAll();

	// add emitter should only be called after Load2D to get the correct sound loaded, then use that to bind to the AddEmitter. 
	std::shared_ptr<SoundEmitter> CreateEmitter(std::shared_ptr<Sound>& sound, vec3 pos = vec3(0, 0, 0));// everything is a point emitter, so there is no direction, or orientation
	std::shared_ptr<Sound> LoadSound(std::string file);// this will pre load the sound file and return the id for future use
	class Sound_Starter{
	public:
		Sound_Starter();
		~Sound_Starter();
	};
};


#endif