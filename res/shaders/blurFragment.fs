#version 330 core

uniform vec4 timeColor;
uniform sampler2D screenTexture;

in vec2 vTexCoord;


out vec4 color;

vec3 sharpen(vec2 st){
	vec2 iRes = textureSize(screenTexture, 0);
	float ResS = float(iRes.s);
	float ResT = float(iRes.t);

	vec2 right = vec2(1/ResS, 0);
	vec2 bottom = vec2(0, 1/ResT);
	vec2 topRight = vec2(1/ResS, -(1/ResT));
	vec2 bottomRight = vec2(1/ResS, 1/ResT);
	vec2 left = -right;
	vec2 top = -bottom;
	vec2 bottomLeft = -topRight;
	vec2 topLeft = -bottomRight; 

	vec3 i00 = texture2D(screenTexture, st).rgb;
	vec3 im1m1 = texture2D(screenTexture, st+topLeft).rgb;
	vec3 ip1p1 = texture2D(screenTexture, st+bottomRight).rgb;
	vec3 im1p1 = texture2D(screenTexture, st+bottomLeft).rgb;
	vec3 ip1m1 = texture2D(screenTexture, st+topRight).rgb;
	vec3 im10 = texture2D(screenTexture, st+left).rgb;
	vec3 ip10 = texture2D(screenTexture, st+right).rgb;
	vec3 i0m1 = texture2D(screenTexture, st+top).rgb;
	vec3 i0p1 = texture2D(screenTexture, st+bottom).rgb;
	vec3 target = vec3(0., 0., 0.);
	target += 1. *(im1m1+ip1m1+ip1p1+im1p1);
	target += 2. *(im10+ip10+i0m1+i0p1);
	target += 4. *(i00);
	target /= 16;
	return target;
}

void main()
{ 
	//vec2 scaledCoord = vTexCoord/1.5;
	vec3 imageSample = texture2D(screenTexture, vTexCoord).rgb;
	color = vec4(mix(sharpen(vTexCoord), imageSample, 0.1), 1.0);
	//color = vec4(1.0, 1.0, 0.0, 1.0);
	//color = vec4(vTexCoord.x, vTexCoord.y, 1.0, 1.0);//texture(churchTexture, vTexCoord); 
};