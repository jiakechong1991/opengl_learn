
#version 330 core 
loayout (location = 0) in vec3 aPos;
out vec4 vertextColor;

void main(){
    gl_Postion = vec4(aPos, 1);
    vertextColor = vec4(0.5, 0,0,1);
}


#version 330 core 
in vec4 vertextColor;
out vec4 fragment

void main(){
    fragment = vertextColor;
}