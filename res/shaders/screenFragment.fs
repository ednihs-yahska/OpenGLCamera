#version 330 core

uniform vec4 timeColor;
uniform mat4 uInverseViewProjection;
uniform mat4 uPreviousProjectionM;
uniform sampler2D screenTexture;

in vec2 vTexCoord;


out vec4 color;

vec4 DownSampleFrame(sampler2D uniformSampler, vec2 TexCoords)
{
	vec2 iRes = textureSize(screenTexture, 0);
    vec2 pixelOffset = vec2(1/iRes.s, 1/iRes.t);
	float scaleFactor = 4.0;
    vec3 downScaleColor = vec3(0.0f, 0.0f, 0.0f);
    downScaleColor += texture(uniformSampler, vec2(TexCoords.x - scaleFactor * pixelOffset.x, TexCoords.y)).xyz;
    downScaleColor += texture(uniformSampler, vec2(TexCoords.x + scaleFactor * pixelOffset.x, TexCoords.y)).xyz;
    downScaleColor += texture(uniformSampler, vec2(TexCoords.x, TexCoords.y - scaleFactor * pixelOffset.y)).xyz;
    downScaleColor += texture(uniformSampler, vec2(TexCoords.x, TexCoords.y + scaleFactor * pixelOffset.y)).xyz;
    downScaleColor *= 0.25f;
    return (vec4(downScaleColor, 1.0f));
}

void main()
{ 
	float zOverW = texture(screenTexture, vTexCoord).z;
	vec4 H = vec4(vTexCoord.x * 2 - 1, (1 - vTexCoord.y) * 2 - 1, zOverW, 1);
	vec4 D = H * uInverseViewProjection;
	vec4 worldPos = D / D.w;
	vec4 prevPosition = H * uPreviousProjectionM;
	prevPosition /= prevPosition.w;
	vec4 velocity =  (H - prevPosition)/2.f;
	color = velocity;//DownSampleFrame(screenTexture, vTexCoord); 
};