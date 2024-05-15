#version 460


layout(location = 0) rayPayloadInEXT vec3 hitValue;
hitAttributeEXT vec3 attribs;

void main()
{
  hitValue = vec3(0.2, 0.5, 0.5);
}
