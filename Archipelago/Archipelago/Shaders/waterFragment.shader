#version 330 core

in vec3 fragmentNormal;
in vec3 fragmentPos;
in vec2 TexCoord;

out vec4 color;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 viewerPos;

uniform sampler2D waterTexture;

uniform float time;


void main()
{
	vec3 waterColor = vec3(0.0, 0.0, 1.0);

	//ambient lighting
	float ambientStrength = 0.05f;
    	vec3 ambient_contribution = ambientStrength * lightColor;

	//diffuse lighting
	float incident_degree = max(dot(fragmentNormal, lightDirection), 0.0f);
	vec3 diffuse_contribution = incident_degree * lightColor;

	//specular lighting
	float specularStrength = 1.0f;
	vec3 viewDir = normalize(viewerPos - fragmentPos);
	vec3 reflectDir = normalize(reflect(-lightDirection, fragmentNormal));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular_contribution = specularStrength * spec * lightColor;  

	vec3 finalColor = (ambient_contribution + diffuse_contribution + specular_contribution) * waterColor;

	//FOG:
	//color = vec4(finalColor, 1.0f);
	vec3 difference = fragmentPos - viewerPos;
	float distance = length(difference)/200;
	
	//water moving
	float ampOfWave = 0.1f;
	float period = 0.1f;
	float frequency = 2*3.1415;
	color = (texture(waterTexture, TexCoord + normalize(fragmentPos).xz * ampOfWave * sin(period * length(fragmentPos.xz) - frequency * time)) * vec4(finalColor, 1.0f)); 
	float f = (pow(distance, 4) / 200);
	color = clamp(color*(1-f) + (vec4(0.5f, 0.5f, 0.5f, 1.0f)) * f, 0.0f, 1.0f);
	color.a = 0.99;

}