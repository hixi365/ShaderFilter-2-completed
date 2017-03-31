
//==================================================================================
//	�V�F�[�_�ϐ�
//==================================================================================
cbuffer ConstantBuffer		: register(b0)
{
	float4	g_Float4A;			// �ėp(XMFLOAT4)
	float4	g_Float4B;			// �ėp(XMFLOAT4)
	float4	g_Float4C;			// �ėp(XMFLOAT4)
	int4	g_Int4A;			// �ėp(XMINT4)
	int4	g_Int4B;			// �ėp(XMINT4)
	int4	g_Int4C;			// �ėp(XMINT4)
};

// �e�N�X�`��
Texture2D<float4>	g_Tex0	: register(t0);

// �T���v���[
SamplerState		g_Sampler;

//------------------------------------------------------------
// ���_�V�F�[�_�[�ւ̓��͍\����
//------------------------------------------------------------
struct VSINPUT
{
	float3 pos		: POSITION;
	float3 normal	: NORMAL;
	float4 color	: COLOR;
	float2 tex		: TEXCOORD0;
};

//------------------------------------------------------------
// �s�N�Z�� �V�F�[�_�[�ւ̓��͍\����
//------------------------------------------------------------
struct PSINPUT
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
};

//==================================================================================
// ���_�V�F�[�_
//==================================================================================
PSINPUT mainVS(VSINPUT input)
{

	PSINPUT output;

	float4 pos = float4(input.pos, 1);

	output.pos = pos;
	output.tex = float2(input.tex.x, input.tex.y);

	return output;
}

//===========================================================================================
// �s�N�Z���V�F�[�_
//===========================================================================================
float4 mainPS(PSINPUT input) : SV_TARGET
{

	float4 Out;
	float4 Samp;
	
	int width, height;
	
	g_Tex0.GetDimensions(width, height);
	
	int2 size = int2(width, height);
	int3 sp = int3(input.tex * size, 0);
	
	Samp = g_Tex0.Load(sp);
	
	Out.rgb = Samp.rgb;
	Out.a = 1;
	
	return Out;

}

//==================================================================================
// �s�N�Z���V�F�[�_ �ڂ���
//==================================================================================
float4 blurPS(PSINPUT input) : SV_TARGET
{

	int i;

	float4 Out = g_Tex0.Sample(g_Sampler, input.tex);
	float width, height, numoflevel;
	
	g_Tex0.GetDimensions(0, width, height, numoflevel);
	
	float dw = 1.0f / width;
	float dh = 1.0f / height;
	float2 ei[9] = { float2(-dw, -dh), float2(0, -dh), float2(dw, -dh), float2(-dw, 0), float2(0, 0), float2(dw, 0), float2(-dw, dh), float2(0, dh), float2(dw, dh) };
	
	float2 center = input.tex;
	
	float3 s = float3(0, 0, 0);
	for (i = 0; i < 9; i++)
		s += g_Tex0.Sample(g_Sampler, center + ei[i]).rgb;
	
	Out = float4(s / 9.0f, 1);
	
	return Out;

}

//===========================================================================================
// �s�N�Z���V�F�[�_ �K���}�l
//===========================================================================================
float4 gammaPS(PSINPUT input) : SV_TARGET
{

	float4 Out;
	float4 Samp;

	int width, height;

	g_Tex0.GetDimensions(width, height);

	int2 size = int2(width, height);
	int3 sp = int3(input.tex * size, 0);

	Samp = g_Tex0.Load(sp);

	Out.rgb = pow(Samp.rgb, 1.0f / g_Float4A.x);
	Out.a = 1;

	return Out;

}

//==================================================================================
// �s�N�Z���V�F�[�_ ��s��
//==================================================================================
float4 unsharpPS(PSINPUT input) : SV_TARGET
{

	int i;

	float4 Out = g_Tex0.Sample(g_Sampler, input.tex);
	float width, height, numoflevel;
	
	g_Tex0.GetDimensions(0, width, height, numoflevel);
	
	float dw = 1.0f / width;
	float dh = 1.0f / height;
	float2 ei[9] = { float2(-dw, -dh), float2(0, -dh), float2(dw, -dh), float2(-dw, 0), float2(0, 0), float2(dw, 0), float2(-dw, dh), float2(0, dh), float2(dw, dh) };
	
	float2 center = input.tex;
	
	// �T���v�����O
	float4 samp[9];
	for (i = 0; i < 9; i++)
		samp[i] = g_Tex0.Sample(g_Sampler, center + ei[i]);
	
	int f[9] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
	float3 s = 0;
	for (i = 0; i < 9; i++)
		s += (f[i] * g_Float4A.x + (i == 4)) * samp[i].rgb;
	
	Out = float4(s, 1);
	
	return Out;

}

//==================================================================================
// �s�N�Z���V�F�[�_ ���C�t�Q�[��
//==================================================================================
float4 lifegamePS(PSINPUT input) : SV_TARGET
{

	int i;
	int3 ei[9] = { int3(-1, -1, 0), int3( 0, -1, 0), int3( 1, -1, 0),
				   int3(-1,  0, 0), int3( 0,  0, 0), int3( 1,  0, 0),
				   int3(-1,  1, 0), int3( 0,  1, 0), int3( 1,  1, 0), };
	
	float width, height, numoflevel;
	g_Tex0.GetDimensions(0, width, height, numoflevel);

	int2 size = int2(width, height);
	int3 sp = int3(input.tex * size, 0);

	float4 Out = g_Tex0.Load(sp);
	
	int count = 0;
	for (i = 0; i < 9; i++)
		if(i != 4)
			count += g_Tex0.Load(sp + ei[i]).r > 0.0f;
	
	Out = count == 3 ? 1 : count == 2 ? Out : 0;
	
	return Out;

}