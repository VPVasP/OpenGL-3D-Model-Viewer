#version 330 core

out vec4 FragColor;

in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 viewPosition;

uniform samplerCube skybox;

//Refraction indexes
uniform float extMediumIdx; //refraction index of the exiting medium
uniform float entMediumIdx; //refraction index of the entering medium

vec3 calculateReflection()
{
    vec3 viewDirection = normalize(fragPosition - viewPosition);
    vec3 normal = normalize(fragNormal);

    vec3 reflectionDir = reflect(viewDirection, normal);

    return texture(skybox, reflectionDir).rgb;
}

vec3 calculateRefraction()
{
    vec3 viewDirection = normalize(fragPosition - viewPosition);
    vec3 normal = normalize(fragNormal);

    float ratio = extMediumIdx/entMediumIdx;
    vec3 refractionDir = refract(viewDirection, normal, ratio);

    return texture(skybox, refractionDir).rgb;
}

void main()
{
    vec3 color = calculateReflection();
    FragColor = vec4(color, 1.0);
}