#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
};

struct Light {
    vec3 position;
    vec3 color;

    float constant;
    float Linear;
    float quadratic;
};

in vec3 fragPos;
in vec3 fragNormal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform bool hasTexture;
uniform float ambientStrength;
uniform float lightIntensity; // Add light intensity as a uniform

void main() {
    // Ambient
    vec3 ambient;
    if (hasTexture) {
        ambient = ambientStrength * vec3(texture(material.diffuseMap, TexCoords));
    }
    else {
        ambient = ambientStrength * material.ambient;
    }

    // Diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse;
    if (hasTexture) {
        diffuse = diff * vec3(texture(material.diffuseMap, TexCoords));
    }
    else {
        diffuse = diff * material.diffuse;
    }

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular;
    if (hasTexture && textureSize(material.specularMap, 0).x > 0) {
        specular = spec * vec3(texture(material.specularMap, TexCoords));
    }
    else {
        specular = spec * material.specular;
    }

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.Linear * distance +
        light.quadratic * (distance * distance));

    // Apply light intensity to the ambient, diffuse, and specular components
    ambient *= light.color * lightIntensity;
    diffuse *= light.color * lightIntensity;
    specular *= light.color * lightIntensity;

    // Final lighting calculation
    vec3 result = (ambient + diffuse + specular) * attenuation;

    // Gamma correction
    float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}


//#version 330 core
//
//struct Material {
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//    float shininess;
//    sampler2D diffuseMap;
//    sampler2D specularMap;
//    sampler2D normalMap;
//};
//
//struct Light {
//    vec3 position;
//    vec3 color;
//
//    float constant;
//    float linear;
//    float quadratic;
//};
//
//in vec3 fragPos;
//in vec3 fragNormal;
//in vec2 TexCoords;
//
//out vec4 FragColor; // ���յ���ɫ���
//out vec4 AmbientColor; // ��������ɫ���
//out vec4 DiffuseColor; // ��������ɫ���
//out vec4 SpecularColor; // ���淴����ɫ���
//
//uniform Material material;
//uniform Light light;
//uniform vec3 viewPos;
//uniform bool hasTexture;
//uniform float ambientStrength;
//
//void main() {
//    // ������
//    vec3 ambient;
//    if (hasTexture) {
//        ambient = ambientStrength * vec3(texture(material.diffuseMap, TexCoords));
//    }
//    else {
//        ambient = ambientStrength * material.ambient;
//    }
//
//    // ������
//    vec3 norm = normalize(fragNormal);
//    vec3 lightDir = normalize(light.position - fragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse;
//    if (hasTexture) {
//        diffuse = diff * vec3(texture(material.diffuseMap, TexCoords));
//    }
//    else {
//        diffuse = diff * material.diffuse;
//    }
//
//    // ���淴��
//    vec3 viewDir = normalize(viewPos - fragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular;
//    if (hasTexture && textureSize(material.specularMap, 0).x > 0) {
//        specular = spec * vec3(texture(material.specularMap, TexCoords));
//    }
//    else {
//        specular = spec * material.specular;
//    }
//
//    // ˥��
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance +
//        light.quadratic * (distance * distance));
//
//    // �ֱ�Ӧ�ù�����ɫ
//    ambient *= light.color;
//    diffuse *= light.color;
//    specular *= light.color;
//
//    // �������յ���ɫ
//    vec3 result = (ambient + diffuse + specular) * attenuation;
//
//    // gammaУ��
//    float gamma = 2.2;
//    result = pow(result, vec3(1.0 / gamma));
//
//    // �������
//    FragColor = vec4(result, 1.0);
//    AmbientColor = vec4(ambient, 1.0);
//    DiffuseColor = vec4(diffuse, 1.0);
//    SpecularColor = vec4(specular, 1.0);
//}