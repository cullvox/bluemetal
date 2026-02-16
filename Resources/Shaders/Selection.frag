#version 460

layout(location = 0) in flat uint objectID;

layout(location = 0) out uint color;

void main()
{
    color = objectID;
}