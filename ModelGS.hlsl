#include "Model.hlsli"

[maxvertexcount(3)]
void main(
	triangle VSOutput input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.svpos_ = input[i].svpos_;
        element.normal_ = input[i].normal_;
        element.uv_ = input[i].uv_;
		output.Append(element);
	}
}