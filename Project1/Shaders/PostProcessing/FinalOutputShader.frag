#version 330 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D effectTexture;


void main()
{

    vec3 scenecolor =  texture(sceneTexture, uv).rgb;
    vec3 effectColor =  texture(effectTexture, uv).rgb;

	vec3 result = (scenecolor) + (effectColor);



    FragColor = vec4(result,1);
}