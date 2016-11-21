#version 400

in vec4 vertex;
in vec4 normal;
in vec4 texcoord0;
in vec4 color;

out vec4 outcolor;

uniform mat4 mModelView;
uniform mat4 mProjection;
uniform float fElapsedTime;

void main()
{
	outcolor = color;
	gl_Position =  mProjection * mModelView * vec4(vertex.xyz, 1.0);
}