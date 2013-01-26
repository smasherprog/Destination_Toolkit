#include "Common.fx"

#define FILTER_WIDTH 16.0f
#define vFallOff  float4( -5000.0f, -1.5f, -1.5f, -1000.0f )
#define invMax  float2(1.0f/(.10f/FILTER_WIDTH), 1.0f/(.10f/FILTER_WIDTH))  // inverse of the maximum length of the blur vector
#define cLit  float3( .7f, .7f, .7f )
#define vLengthance float2(21600000.0f, 600.0f * (2.0f*21600000.0f + 600.0f ))
#define LightColor float3(1.2f, 1.2f, 1.2f)
#define LightScale 8.0f
#define EarthRadius 21600000.0f
#define AtomosHeight 30000.0f
#define RayLeigh float3(.3f, .45f, 6.5f)
#define Mei float3(.3f, .3f, .3f)


cbuffer perframe{
	float4 vRayleigh;   // rgb : 3/(16*PI) * Br           w : -2*g
	float4 vMie;        // rgb : 1/(4*PI) * Bm * (1-g)^2  w : (1+g^2)
	float4 vESun;       // rgb : Esun/(Br+Bm)             w : R
	float4 vSum;        // rgb : (Br+Bm)                  w : h(2R+h)
	float4 vAmbient;    // rgb : ambient
	float4 vUVParam;
	float4 vXZParam;
	float4 vOff;
	float4 vParam;
	float4 vHeightPix;
	//float4 SunDir;
	float4 CameraPosition;
	float4x4 InvCameraViewProjection;

};
cbuffer perframe1{
	float4x4 curmatrix;
};
struct SVSOutput_Blur {
	float4 vWorldPos : TEXCOORD0;
	float4 vTex : TEXCOORD1;
	float4 vPos : SV_POSITION;
};

Texture2D texture0, texture1;
SamplerState sampler0;

VSQuadOut RenderCloud_Grid_VS(float4 vPos : POSITION){
	VSQuadOut _Output;
	// compute world position
	float4 vWorldPos;
	vWorldPos.xz = vPos.xy * vXZParam.xy + vXZParam.zw;
	// height is propotional to the square Lengthance in horizontal direction.
	float2 vDir = CameraPosition.xz - vWorldPos.xz;
	float fSqLengthance = dot( vDir, vDir );
	vWorldPos.y = (fSqLengthance * vHeightPix.x + vHeightPix.y);
	vWorldPos.w = 1.0f;
	// transform and projection
	_Output.position = mul( vWorldPos, curmatrix);
	
	// texture coordinate 
	_Output.texcoord = vPos.zw * vUVParam.xy + vUVParam.zw; 

	return _Output;
}


float4 RenderCloud_Grid_PS(VSQuadOut _Input) : SV_Target0{
	float4 clTex = texture0.Sample( sampler0, _Input.texcoord ); //linearfilter
	
	// blend 4 channel of the cloud texture according to cloud cover 
	float4 vDensity =  abs( vParam.w - float4( 0.25f, 0.5f, 0.75f, 1.0f ) ) / 0.25f;
	vDensity = saturate( 1.0f - vDensity );
	float _fDensity = dot( clTex, vDensity );
	return _fDensity;
}

VSQuadOutMax RenderCloud_Plane_VS(uint VertexID: SV_VertexID){// ouputs a full screen quad with tex coords
	VSQuadOutMax Out;
    Out.texcoord = float2( (VertexID << 1) & 2, VertexID & 2 );
    Out.position = float4( Out.texcoord * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), .99995f, 1.0f );
	Out.vWorldPos = mul(Out.position, InvCameraViewProjection );// world positon
    return Out;
}

/*
float3 ApplyScattering( float3 _clInput, float3 _vRay ){
	// calcurating in-scattering 	
	float _fVL = dot( SunDir.xyz, -_vRay );
	float fg = Mie.w + Rayleigh.w * _fVL;
	fg = rsqrt( fg );
	fg = fg*fg*fg;
	float3 _vMie = Mie.rgb * fg;
	float3 _vRayleigh = Rayleigh.rgb*(1.0f + _fVL*_fVL);
	float3 _vInscattering = vESun * (_vMie + _vRayleigh) + vAmbient.rgb;
	
	// compute Lengthance to the cloud
	float _fSin = _vRay.y;
	float _fRSin = vLengthance.x * _fSin;
	float _fLengthance = sqrt( _fRSin * _fRSin + vLengthance.y ) - _fRSin;
		
	float3 fRatio = exp( -vSum.rgb * _fLengthance );		
	return lerp( _vInscattering, _clInput, fRatio );
}
*/
float4 RenderCloud_Plane_PS(VSQuadOutMax _Input) : SV_Target0{
	float _clDensity = texture0.Sample(sampler0, _Input.texcoord.xy ).x; //linear filter clamp
	float _clLit     = 1.0f - texture1.Sample( sampler0, _Input.texcoord.xy ).x;//linear filter clamp

	// light cloud
	float3 _clCloud = float3(0.3f, 0.35f, 0.37f) + cLit * _clLit.r;

	// compute ray direction	
	float3 _vWorldPos = _Input.vWorldPos.xyz/_Input.vWorldPos.w;
	float3 _vRay = _vWorldPos - CameraPosition.xyz;	

	_vRay = normalize( _vRay );
		
	// apply scattering 
	//float4 _color;
	//_color.rgb = ApplyScattering( _clCloud, _vRay );
	//_color.rgb = saturate(_color.rgb);
	//_color.a = _clDensity.a;
	float y = _vRay.y * 3 + .6;
	return float4(_clCloud, _clDensity*y);
}


SVSOutput_Blur RenderCloud_Blur_VS(uint VertexID: SV_VertexID){ 
	SVSOutput_Blur _Output;
	float2 te = float2( (VertexID << 1) & 2, VertexID & 2 );
    float4 pos = float4( te * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 1.0f, 1.0f );
	_Output.vPos = float4( pos.xy, 0.5f, 1 );// projection position
	_Output.vWorldPos = mul( pos, InvCameraViewProjection );// transform projection space to world space 	
	_Output.vTex.xy = te + vHeightPix.zw;// uv 
	_Output.vTex.zw = te * (-vOff.xy) + (-vOff.zw);// compute blur direction 
	return _Output;
}


float4 RenderCloud_Blur_PS(SVSOutput_Blur _Input) : SV_Target0{
	// compute ray direction 
	float3 _vWorldPos = _Input.vWorldPos.xyz/_Input.vWorldPos.w;
	float3 _vRay = _vWorldPos - CameraPosition.xyz;	
	float _fSqLengthance = dot( _vRay, _vRay );
	_vRay = normalize( _vRay );

	// compute Lengthance the light passes through the atmosphere.	
	float _fSin = _vRay.y;
	float _fRSin = vParam.x * _fSin;
	float _fLengthance = sqrt( _fRSin * _fRSin + vParam.y ) - _fRSin;

	// Compute UV offset.
	float2 _vUVOffset = _Input.vTex.zw / FILTER_WIDTH * (vParam.z / _fLengthance);

	// limit blur vector
	float2 _len = abs( _vUVOffset * invMax );
	float _over = max( _len.x, _len.y );
	float _scale = _over > 1.0f ? 1.0f/_over : 1.0f;
	_vUVOffset.xy *= _scale;

	// scale parameter of exponential weight
	float4 _Lengthance;
	_Lengthance = dot( _vUVOffset.xy, _vUVOffset.xy );
	_Lengthance *= vFallOff;

	// blur 
	float2 _uv = _Input.vTex.xy;
	float4 _clOut = texture0.Sample( sampler0, _uv ).x;//LinearFilterClamp

	float4 _fWeightSum = 1.0f;
	for ( int i = 1; i < FILTER_WIDTH; ++i ) {
		float4 _weight = exp( _Lengthance * i );
		_fWeightSum += _weight;
		
		float2 _vMove = _vUVOffset * i;		
		float4 _clDensity = texture0.Sample( sampler0,  _uv + _vMove );
		_clOut += _weight * _clDensity;
	}
	_clOut /= _fWeightSum;

	return _clOut;
}

float4 RenderCloud_Shadow_PS(VSQuadOut _Input) : SV_Target0{
	float4 clTex = texture0.Sample( sampler0, _Input.texcoord );//linearfilter
	// blend 4 channel of the cloud texture according to cloud cover 
	float4 vDensity =  abs( vParam.w - float4( 0.25f, 0.5f, 0.75f, 1.0f ) ) / 0.25f;
	vDensity = saturate( 1.0f - vDensity );
	float _fDensity = dot( clTex, vDensity );
	return 1.0f-_fDensity;
	
}



 