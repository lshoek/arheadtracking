#version 400

in vec4 outcolor;

out vec4 fragcolor;

uniform mat4 mModelView;
uniform mat4 mProjection;
uniform float fElapsedTime;

void main()
{
	fragcolor = outcolor;
}