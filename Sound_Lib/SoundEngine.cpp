#include "SoundEngine.h"
#include <fstream>

IXAudio2* Sound_Engine::Internal::pXAudio2=0;
IXAudio2MasteringVoice* Sound_Engine::Internal::pMasteringVoice=0;
IXAudio2SubmixVoice* Sound_Engine::Internal::pSubmixVoice=0;
IUnknown* Sound_Engine::Internal::pReverbEffect=0;

//3d stuff
X3DAUDIO_HANDLE Sound_Engine::Internal::x3DInstance;
int Sound_Engine::Internal::nFrameToApply3DAudio=0;

DWORD Sound_Engine::Internal::dwChannelMask=0;
UINT32 Sound_Engine::Internal::nChannels=0;

X3DAUDIO_DSP_SETTINGS Sound_Engine::Internal::dspSettings;
X3DAUDIO_LISTENER Sound_Engine::Internal::listener;
std::vector<std::shared_ptr<Sound_Engine::SoundEmitter>> Sound_Engine::Internal::Emitters;
std::map<std::string, std::weak_ptr<Sound_Engine::Internal::Sub_Sound>> Sound_Engine::Internal::LoadedSounds;
std::map<Sound_Engine::Sound*, std::weak_ptr<Sound_Engine::Sound>> Sound_Engine::Internal::Sources;

D3DXVECTOR3 Sound_Engine::Internal::CurrentListenerPos = D3DXVECTOR3(0, 0, 0);
FLOAT32 Sound_Engine::Internal::matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS] = {0, 0, 0, 0, 0, 0, 0, 0};



// little endian format
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

#define CALCULATION3D X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB | X3DAUDIO_CALCULATE_REVERB | X3DAUDIO_CALCULATE_REDIRECT_TO_LFE 
// Specify sound cone to add directionality to listener for artistic effect:
// Emitters behind the listener are defined here to be more attenuated,
// have a lower LPF cutoff frequency,
// yet have a slightly higher reverb send level.
static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI*5.0f/6.0f, X3DAUDIO_PI*11.0f/6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

// Specify LFE level distance curve such that it rolls off much sooner than
// all non-LFE channels, making use of the subwoofer more dramatic.
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_LFE_Curve          = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_LFE_CurvePoints[0], 3 };

// Specify reverb send level distance curve such that reverb send increases
// slightly with distance before rolling off to silence.
// With the direct channels being increasingly attenuated with distance,
// this has the effect of increasing the reverb-to-direct sound ratio,
// reinforcing the perception of distance.
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve          = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0], 3 };

// Must match order of g_PRESET_NAMES
XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS[ NUM_PRESETS ] =
{
	XAUDIO2FX_I3DL2_PRESET_FOREST,
	XAUDIO2FX_I3DL2_PRESET_DEFAULT,
	XAUDIO2FX_I3DL2_PRESET_GENERIC,
	XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
	XAUDIO2FX_I3DL2_PRESET_ROOM,
	XAUDIO2FX_I3DL2_PRESET_BATHROOM,
	XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
	XAUDIO2FX_I3DL2_PRESET_STONEROOM,
	XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
	XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
	XAUDIO2FX_I3DL2_PRESET_CAVE,
	XAUDIO2FX_I3DL2_PRESET_ARENA,
	XAUDIO2FX_I3DL2_PRESET_HANGAR,
	XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
	XAUDIO2FX_I3DL2_PRESET_HALLWAY,
	XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
	XAUDIO2FX_I3DL2_PRESET_ALLEY,
	XAUDIO2FX_I3DL2_PRESET_CITY,
	XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
	XAUDIO2FX_I3DL2_PRESET_QUARRY,
	XAUDIO2FX_I3DL2_PRESET_PLAIN,
	XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
	XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
	XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
	XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
	XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
	XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
	XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
	XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
	XAUDIO2FX_I3DL2_PRESET_PLATE,
};


Sound_Engine::Sound::Sound(): SourceVoice(0) { }
Sound_Engine::Sound::~Sound(){
	Internal::Sources.erase(this);
	if(SourceVoice) SourceVoice->DestroyVoice();
}
void Sound_Engine::Sound::Pause(){  HR(SourceVoice->Stop()); }
void Sound_Engine::Sound::Stop(){ HR(SourceVoice->FlushSourceBuffers());}
void Sound_Engine::Sound::Play(bool loopforever ){
	if(loopforever) InternalSound->buffer.LoopCount =XAUDIO2_LOOP_INFINITE;// loop over and over

	if(!SourceVoice)// the sound buffer has already been allocated, no need to reallocate it
		HR(Internal::pXAudio2->CreateSourceVoice( &SourceVoice, (WAVEFORMATEX*)&(InternalSound->wfx)) );	
	HR(SourceVoice->SubmitSourceBuffer( &InternalSound->buffer, &InternalSound->wmaBuffer ) );
	HR(SourceVoice->Start() );

}
void Sound_Engine::Sound::Resume(){ HR(SourceVoice->Start()); }

void Sound_Engine::PauseAll(){ Sound_Engine::Internal::pXAudio2->StopEngine(); }
void Sound_Engine::ResumeAll(){ Sound_Engine::Internal::pXAudio2->StartEngine(); }
void Sound_Engine::StopAll(){ 
	for(auto beg = Internal::Sources.begin(); beg != Internal::Sources.end(); beg++){
		std::shared_ptr<Sound> ptr(beg->second.lock());
		if(ptr) {
			if(ptr->SourceVoice) ptr->SourceVoice->Stop();
		}
	}
}

void Sound_Engine::Internal::Init(){
	pXAudio2=0;
	pMasteringVoice=0;
	pSubmixVoice=0;
	pReverbEffect=0;
	nFrameToApply3DAudio=0;
	dwChannelMask=0;
	nChannels=0;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HR(XAudio2Create( &pXAudio2, 0 )) ;
	HR(pXAudio2->CreateMasteringVoice( &pMasteringVoice ) );

	// Check device details to make sure it's within our sample supported parameters
	XAUDIO2_DEVICE_DETAILS details;
	HR(pXAudio2->GetDeviceDetails( 0, &details ) );
	if( details.OutputFormat.Format.nChannels > OUTPUTCHANNELS ){
		RELEASECOM( pXAudio2 );
		assert(true);
	}
	dwChannelMask = details.OutputFormat.dwChannelMask;
	nChannels = details.OutputFormat.Format.nChannels;

	HR(XAudio2CreateReverb( &pReverbEffect, 0 ) ); // Create reverb effect


	// Create a submix voice
	// Performance tip: you need not run global FX with the sample number
	// of channels as the final mix.  For example, this sample runs
	// the reverb in mono mode, thus reducing CPU overhead.
	XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { pReverbEffect, TRUE, 1 } };
	XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };
	HR(pXAudio2->CreateSubmixVoice( &pSubmixVoice, 1,details.OutputFormat.Format.nSamplesPerSec, 0, 0, NULL, &effectChain ) );

	// Set default FX params
	XAUDIO2FX_REVERB_PARAMETERS native;
	ReverbConvertI3DL2ToNative( &g_PRESET_PARAMS[0], &native );
	pSubmixVoice->SetEffectParameters( 0, &native, sizeof( native ) );

	//
	// Initialize X3DAudio
	//  Speaker geometry configuration on the final mix, specifies assignment of channels
	//  to speaker positions, defined as per WAVEFORMATEXTENSIBLE.dwChannelMask

	X3DAudioInitialize( details.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance );
	CurrentListenerPos = D3DXVECTOR3( 0, 0, 0 );

	listener.Position = CurrentListenerPos;
	listener.OrientFront = D3DXVECTOR3( 0, 0, 1 );
	listener.OrientTop = D3DXVECTOR3( 0, 1, 0 );
	listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

	dspSettings.SrcChannelCount = INPUTCHANNELS;
	dspSettings.DstChannelCount = nChannels;
	dspSettings.pMatrixCoefficients = matrixCoefficients;
}
void Sound_Engine::Internal::DeInit(){
	Emitters.clear();
	LoadedSounds.clear();
	Sources.clear();
	if( pSubmixVoice ){
		pSubmixVoice->DestroyVoice();
		pSubmixVoice = NULL;
	}
	if( pMasteringVoice ) {
		pMasteringVoice->DestroyVoice();
		pMasteringVoice = NULL;
	}
	if(pXAudio2){
		pXAudio2->StopEngine();
		RELEASECOM( pXAudio2 );
	}
	RELEASECOM(pReverbEffect);
	CoUninitialize();
}


bool FindChunk(std::ifstream& stream, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition){
	stream.seekg(0, std::ios::beg);
	DWORD dwChunkType(0), dwChunkDataSize(0), dwRIFFDataSize(0), dwFileType(0), dwOffset(0);
	while(!stream.eof()){// as long as we have not hit the end of the file
		// riff standard is always type, followied by the chunk size. They are always 4 bytes too, 
		stream.read(reinterpret_cast<char*>(&dwChunkType), 4);
		stream.read(reinterpret_cast<char*>(&dwChunkDataSize), 4);
		if( fourccRIFF ==dwChunkType){// at the riff header
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			stream.read(reinterpret_cast<char*>(&dwFileType), 4);              
		} else stream.seekg(dwChunkDataSize, std::ios::cur);// skip this chunk
		dwOffset += sizeof(DWORD) * 2; 
		if (dwChunkType == fourcc){// found what we were looking for
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return true;
		}
		dwOffset += dwChunkDataSize;
	}
	char* temp = reinterpret_cast<char*>(&fourcc);
	OUTPUT_DEBUG_MSG("Could not find the chunk "<<temp[0]<<temp[1]<<temp[2]<<temp[3]);
	return false;// if this is hit, it means what we were searching for was not found
}
void ReadChunkData(std::ifstream& stream, void* buffer, DWORD buffersize, DWORD bufferoffset){
	stream.seekg(bufferoffset, std::ios::beg);
	stream.read(reinterpret_cast<char*>(buffer), buffersize); 
}
std::shared_ptr<Sound_Engine::Sound> Sound_Engine::LoadSound(std::string file){
	std::string::size_type po = file.find_last_of('\\');
	if(po == std::string::npos) po = file.find_last_of('/');// couldnt find it with the double slashes, try a single forward slash
	if(po == std::string::npos) {// no slashes.. there must be no path on this string so append our asset directory to where the sounds are stored
		file = Internal::Asset_Dir + "Sound\\" + file;
	}// else the user specified some sort of directory, so use that instead.
		
	std::ifstream f(file.c_str(), std::ios::beg | std::ios::binary);
	assert(f);
	// see if the sound data has been loaded already
	std::map<std::string, std::weak_ptr<Internal::Sub_Sound>>::iterator soundloaded = Internal::LoadedSounds.find(file);
	if(soundloaded != Internal::LoadedSounds.end()) {// the data exists
		std::shared_ptr<Internal::Sub_Sound> ptr(soundloaded->second.lock());
		if(ptr){// the sound data still exists
			std::shared_ptr<Sound_Engine::Sound> sound_ptr = std::make_shared<Sound_Engine::Sound>();// create a new source 
			sound_ptr->InternalSound = ptr;
			Internal::Sources[sound_ptr.get()] = sound_ptr;// insert into the sources
			return sound_ptr;
		}
	}
	std::shared_ptr<Sound_Engine::Sound> retsound = std::make_shared<Sound_Engine::Sound>();
	Internal::Sources[retsound.get()] = retsound;
	retsound->InternalSound = std::make_shared<Internal::Sub_Sound>();
	Internal::LoadedSounds[file]=retsound->InternalSound;

	retsound->InternalSound->buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	DWORD dwChunkSize(0);
	DWORD dwChunkPosition(0);
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(f,fourccRIFF,dwChunkSize, dwChunkPosition );
	DWORD filetype(0);
	ReadChunkData(f, &filetype, sizeof(DWORD), dwChunkPosition);
	if (filetype != fourccWAVE){
		if(filetype != fourccXWMA){
			f.close();
			OUTPUT_DEBUG_MSG("File is not a .wav, or a xwma file... uh oh");
			assert(true);
		}
	}
	FindChunk(f,fourccFMT, dwChunkSize, dwChunkPosition );
	ReadChunkData(f, &retsound->InternalSound->wfx, dwChunkSize, dwChunkPosition );
	FindChunk(f,fourccDATA,dwChunkSize, dwChunkPosition );
	retsound->InternalSound->buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
	BYTE *temp = new BYTE[dwChunkSize];  //size of the audio buffer in bytes
	retsound->InternalSound->buffer.pAudioData = temp;
	ReadChunkData(f, temp, dwChunkSize, dwChunkPosition);
	if(filetype == fourccXWMA){// the file is an xwma file... we need an extra peice of data
		FindChunk(f,fourccDPDS, dwChunkSize, dwChunkPosition );
		// the chunksize is how many bytess there are, so we have to divide by sizeof(uint32t) to get the number of uint32's in the buffer
		//so chunksize will be like 24, but that is how many bytes, we need 24/4 which is 6. Because there are 6 uint32's
		retsound->InternalSound->wmaBuffer.PacketCount = dwChunkSize / sizeof(UINT32); 
		UINT32 *temp2 = new UINT32[retsound->InternalSound->wmaBuffer.PacketCount];
		ReadChunkData(f, temp2, dwChunkSize, dwChunkPosition);
		retsound->InternalSound->wmaBuffer.pDecodedPacketCumulativeBytes= temp2;
	}
	f.close();
	return retsound;
}

std::shared_ptr<Sound_Engine::SoundEmitter> Sound_Engine::CreateEmitter(std::shared_ptr<Sound>& sound, vec3 pos){
	assert(sound);
	assert(sound->InternalSound);
	std::shared_ptr<Sound_Engine::SoundEmitter> ptr = std::make_shared<Sound_Engine::SoundEmitter>();
	ptr->sound = std::make_shared<Sound>();
	Internal::Sources[ptr->sound.get()] = ptr->sound;// insert into the sources
	ptr->sound->InternalSound = sound->InternalSound;// copy the internals

	ptr->Active=true;
	ptr->emitter.pCone = &(ptr->emitterCone);
	ptr->emitter.pCone->InnerAngle = 0.0f;
	// Setting the inner cone angles to X3DAUDIO_2PI and
	// outer cone other than 0 causes
	// the emitter to act like a point emitter using the
	// INNER cone settings only.
	ptr->emitter.pCone->OuterAngle = 0.0f;
	// Setting the outer cone angles to zero causes
	// the emitter to act like a point emitter using the
	// OUTER cone settings only.
	ptr->emitter.InnerRadius = 0.0f;
	ptr->emitter.InnerRadiusAngle = 0.0f;
	ptr->emitter.pCone->InnerVolume = 0.0f;
	ptr->emitter.pCone->OuterVolume = 1.0f;
	ptr->emitter.pCone->InnerLPF = 0.0f;
	ptr->emitter.pCone->OuterLPF = 1.0f;
	ptr->emitter.pCone->InnerReverb = 0.0f;
	ptr->emitter.pCone->OuterReverb = 1.0f;

	ptr->emitter.Position = *reinterpret_cast<D3DXVECTOR3*>(&pos);
	ptr->emitter.OrientFront = D3DXVECTOR3( 0, 0, 1 );
	ptr->emitter.OrientTop = D3DXVECTOR3( 0, 1, 0 );
	ptr->emitter.ChannelCount = INPUTCHANNELS;
	ptr->emitter.ChannelRadius = 1.0f;
	ptr->emitter.pChannelAzimuths = ptr->emitterAzimuths;

	// Use of Inner radius allows for smoother transitions as
	// a sound travels directly through, above, or below the listener.
	// It also may be used to give elevation cues.
	ptr->emitter.InnerRadius = 2.0f;
	ptr->emitter.InnerRadiusAngle = X3DAUDIO_PI/4.0f;

	ptr->emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
	ptr->emitter.pLFECurve    = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
	ptr->emitter.pLPFDirectCurve = NULL; // use default curve
	ptr->emitter.pLPFReverbCurve = NULL; // use default curve
	ptr->emitter.pReverbCurve    = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
	ptr->emitter.CurveDistanceScaler = 14.0f;
	ptr->emitter.DopplerScaler = 1.0f;
	Internal::Emitters.push_back(ptr);
	return ptr;

}
Sound_Engine::Internal::Sub_Sound::Sub_Sound(){
	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	memset(&wmaBuffer, 0, sizeof(XAUDIO2_BUFFER_WMA));	
}
Sound_Engine::Internal::Sub_Sound::~Sub_Sound(){
	Internal::LoadedSounds.erase(File_Name);
	DELETE_ARRAY(buffer.pAudioData); 
	DELETE_ARRAY(wmaBuffer.pDecodedPacketCumulativeBytes); 
}
void Sound_Engine::Internal::SetReverb( uint32_t nReverb ){ 
	if(nReverb > NUM_PRESETS) return; 
	if( pSubmixVoice ){
		XAUDIO2FX_REVERB_PARAMETERS native;
		ReverbConvertI3DL2ToNative( &g_PRESET_PARAMS[ nReverb ], &native );
		pSubmixVoice->SetEffectParameters( 0, &native, sizeof( native ) );
	}
}
void Sound_Engine::Init(){ Internal::Init(); }
void Sound_Engine::DeInit(){ Internal::DeInit(); }
void Sound_Engine::Update(float dt) { Internal::Update(dt); }
void Sound_Engine::SetReverb( uint32_t nReverb ) { Internal::SetReverb(nReverb); } 
void Sound_Engine::SetListenerPositionAndOrientation(vec3& pos, vec3 look , vec3 up) {Internal::SetListenerPositionAndOrientation(pos, look, up); }

void Sound_Engine::Internal::SetListenerPositionAndOrientation(vec3& pos, vec3 look, vec3 up){ 
	CurrentListenerPos = *reinterpret_cast<D3DXVECTOR3*>(&pos);
	listener.OrientFront = *reinterpret_cast<D3DXVECTOR3*>(&look);
	listener.OrientTop = *reinterpret_cast<D3DXVECTOR3*>(&up);
}

void Sound_Engine::Internal::Update(float dt){
	if( nFrameToApply3DAudio == 0 ){// apply settings every other frame  
		//update my position and velicoty
		D3DXVECTOR3 lVelocity = ( CurrentListenerPos - listener.Position ) / dt;
		listener.Position = CurrentListenerPos;
		listener.Velocity = lVelocity;
		// update each emitter
		std::vector<int> indextoremove;
		for(size_t i(0); i<Emitters.size(); i++){
			if(Emitters[i].use_count() == 1) {
				indextoremove.push_back(i);
				continue;
			}
	
			if(!Emitters[i]->Active) continue;// skip this emitter, it is not active
			D3DXVECTOR3 eVelocity = ( Emitters[i]->Pos - Emitters[i]->emitter.Position ) / dt;
			Emitters[i]->emitter.Position = Emitters[i]->Pos;
			Emitters[i]->emitter.Velocity = eVelocity;

			X3DAudioCalculate( x3DInstance, &listener, &Emitters[i]->emitter, CALCULATION3D, &dspSettings );
			// Apply X3DAudio generated DSP settings to XAudio2

			Emitters[i]->sound->SourceVoice->SetFrequencyRatio( dspSettings.DopplerFactor );
			Emitters[i]->sound->SourceVoice->SetOutputMatrix( pMasteringVoice, INPUTCHANNELS, nChannels, matrixCoefficients );

			Emitters[i]->sound->SourceVoice->SetOutputMatrix(pSubmixVoice, 1, 1, &dspSettings.ReverbLevel);

			XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
			Emitters[i]->sound->SourceVoice->SetOutputFilterParameters(pMasteringVoice, &FilterParametersDirect);
			XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
			Emitters[i]->sound->SourceVoice->SetOutputFilterParameters(pSubmixVoice, &FilterParametersReverb);
		}
		for(size_t i =0; i< indextoremove.size(); i++){
			Emitters.erase(Emitters.begin() + i);
		}
	}
	nFrameToApply3DAudio++;
	nFrameToApply3DAudio &= 1;

}
Sound_Engine::Sound_Starter::Sound_Starter(){ Init(); }
Sound_Engine::Sound_Starter::~Sound_Starter(){ DeInit(); }
