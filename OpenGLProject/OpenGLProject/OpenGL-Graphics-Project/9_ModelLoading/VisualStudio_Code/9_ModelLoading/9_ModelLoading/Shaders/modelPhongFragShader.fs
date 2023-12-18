#version 330 core

out vec4 FragColor;

uniform float ambientCoeff;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

in vec2 texCoordinates;
in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform bool useBlinnPhong;

void main()
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Ambient component
    ambient = ambientCoeff * texture(texture_diffuse1, texCoordinates).rgb;

    // Diffuse component
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(lightPosition - fragPosition);
    diffuse = texture(texture_diffuse1, texCoordinates).rgb * max(dot(normal, lightDirection), 0.0);

    // Specular component
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    if(useBlinnPhong)
    {
        vec3 halfwayVector = normalize(lightDirection + viewDirection);
        specular = texture(texture_specular1, texCoordinates).rgb * pow(max(dot(halfwayVector, normal), 0.0), shininess);
    }
    else
    {
        vec3 reflectionDirection = reflect(-lightDirection, normal);
        specular = texture(texture_specular1, texCoordinates).rgb * pow(max(dot(viewDirection, reflectionDirection), 0.0), shininess);
    }

    vec3 result = (ambient + diffuse + specular) * lightColor;
    FragColor = vec4(result, 1.0);
}