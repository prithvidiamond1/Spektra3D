
#version 330 core

precision highp float;

in float vertexHeight;

out vec4 fragColor;

uniform float maxHeight;

vec3 mapHeightToColor(float height){
	float normHeight = height / maxHeight; // Normalize the value to [0, 1]

    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 blue = vec3(0.0, 0.0, 1.0);

    vec3 color = mix(red, blue, normHeight); // Blend between red and blue
    return color;
}

void main()
{
    
    // calculate alpha value per fragment
    float r = 0.0, delta = 0.0, alpha = 1.0;
    vec2 cxy = 2.0 * gl_PointCoord - 1.0;
    r = dot(cxy, cxy);
    delta = fwidth(r);

    alpha = 1.0 - smoothstep(1.0 - delta, 1.0 + delta, r);

    vec3 finalColor = mapHeightToColor(vertexHeight);
    fragColor = vec4(finalColor, alpha);
}

